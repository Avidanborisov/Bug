.intel_syntax noprefix
.code16

.data
rootSectors: .word 0 # number of sectors in the root directory
rootStart:   .word 0 # first root entry sector

notFound:    .string "NF!"

BYTES_PER_ENTRY = 32    # size of FAT12 file entry
CLUSTER_OFFSET  = 26    # offset of the cluster in the file entry structor
FILENAME_LENGTH = 11
CLUSTER_EOF     = 0xff8 # This cluster value (or more) means we have  reached the end of the cluster chain

.text

/**
 * Calculate the size and first file sector of the root directory
 */
.global loadRootInfo
loadRootInfo:
    # root sectors = (bytes per entry * number of root entries) / sector size
    mov ax, BYTES_PER_ENTRY
    xor dx, dx
    mul word ptr numRootEntries
    div word ptr sectorSize
    mov rootSectors, ax

    # rootStart = (num of FAT tables * sectors per FAT) + reserved sectors + hidden sectors
    mov ax, word ptr sectorsPerFAT
    mul byte ptr numFATs
    add ax, numHiddenSectors
    adc ax, numHiddenSectors + 2 # high word
    add ax, numReservedSectors
    mov rootStart, ax

    ret

/**
 * Find a file with filename at ds:si. Expects es to specify segment to load file entries into.
 * Return file cluster number.
 */
.global findFile
findFile:
    mov ax, rootStart
    mov cx, rootSectors

nextSector:
    push cx
    xor bx, bx
    call readSector

checkEntry:
    push si
    mov cx, FILENAME_LENGTH
    mov di, bx
    repe cmpsb
    pop si
    je found

    add bx, BYTES_PER_ENTRY
    cmp bx, word ptr sectorSize
    jne checkEntry

    inc ax # check next sector
    pop cx # cx is now the number of sectors left in root dir.
    loopnz nextSector # loop till it's zero (check all sectors in the root dir)

    # File not found
    mov si, offset notFound
    call puts
    call reboot

found:
    pop cx

    mov ax, es:[bx + CLUSTER_OFFSET] # get the cluster member from the entry found
    ret

/**
 * Load FAT into memory segment specified by es.
 */
.global readFAT
readFAT:
    xor bx, bx

    # Calculate offset of FAT
    mov ax, numReservedSectors
    add ax, word ptr numHiddenSectors
    adc ax, word ptr numHiddenSectors + 2 # high word

    # Read all FAT sectors into memory
    mov cx, sectorsPerFAT
    call readSectors

    ret

/**
 * Convert FAT12 cluster number at ax to its LBA.
 */
cluster2lba:
    add ax, rootStart
    add ax, rootSectors
    sub ax, 2
    ret

/**
 * Compute next cluster number using cluster at cx and FAT
 * table loaded at segment specified by ax.
 */
getNextCluster:
    push ds
    mov ds, ax

    mov si, cx
    mov dx, cx
    shr si, 1
    add si, dx

    mov dx, ds:[si]
    test cx, 1 # is current cluster even?
    jnz oddCluster

evenCluster:
    and dh, 0x0f
    jmp done

oddCluster:
    shr dx, 4

done:
    mov cx, dx
    pop ds
    ret

/**
 * Read a file from the FAT12 filesystem into the memory segment specified by es, using
 * the first file cluster index specified by cx.
 * Assumes the FAT table is already loaded to the memory segment specified by ax
 */
.global readFile
readFile:
    xor bx, bx

nextCluster:
    push ax
    push cx

    mov ax, cx
    call cluster2lba
    movzx cx, byte ptr sectorsPerCluster
    call readSectors

    pop cx
    pop ax

    call getNextCluster
    cmp cx, CLUSTER_EOF
    jl nextCluster

    ret
