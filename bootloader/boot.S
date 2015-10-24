.intel_syntax noprefix
.code16

FAT_SEGMENT  = 0x07e0 # = load address + sectorSize = 0x7xc00 + 512
                      # segment to load the FAT table to.

LOAD_SEGMENT = 0x0900 # = FAT_SEGMENT + sectorsPerFAT * sectorSize = 0x7e00 + 9*512
                      # segment to load the second stage binary to.

.text
.global start
start:
    jmp stub
    nop

/**
 * The BIOS Parameter Block, required on FAT12 filesystems.
 *
 * Note: The volume label here is used to specify the file name of the second stage
 * binary. This is done to save space.
 */
biosParameterBlock:
    oem:                 .ascii  "Bug     "    # OEM String (8 chars max, padded with zeros)
    sectorSize:          .word   512           # Bytes per sector
    sectorsPerCluster:   .byte   1             # Sectors per cluster
    numReservedSectors:  .word   1             # Number of reserved sectors
    numFATs:             .byte   2             # Number of FAT copies
    numRootEntries:      .word   224           # Number of entries in root directory
    numSmallSectors:     .word   2880          # 16-bit sector count (used if numLargeSectors == 0)
    mediaType:           .byte   0xf0          # Media type descriptor (0xf0 for 1.44MiB floppy)
    sectorsPerFAT:       .word   9             # Sectors per each FAT
    sectorsPerTrack:     .word   18            # Sectors per track
    numHeads:            .word   2             # Number of heads
    numHiddenSectors:    .int    0             # Number of hidden sectors
    numLargeSectors:     .int    0             # 32-bit sector count (used if numSmallSectors == 0)

extendedBiosParameterBlock:
    bootDrive:           .byte   0             # Will hold the BIOS drive number that the boot sector came from
    reserved:            .byte   0             # Reserved
    signature:           .byte   0x29          # Extended boot sector signature
    volumeSerial:        .int    0             # Volume serial number
    fileName:            .ascii  "KERNEL  BIN" # Volume label (11 chars max, padded with zeros). Here it's the second stage file name.
    fsType:              .ascii  "FAT12   "    # File system type (8 chars max, padded with zeros)

.global bootDrive, sectorsPerTrack, numHeads, sectorSize, numRootEntries, \
        numFATs, sectorsPerFat, numHiddenSectors, numReservedSectors, \
        sectorsPerFAT, sectorsPerCluster

stub:
    jmp 0:main # enforce CS = 0 because some BIOS' load the boot sector to 0x7c0:0 instead of 0:0x7c00

/**
 * The main bootloader code. Loads a second stage binary image to LOAD_SEGMENT
 * and jumps to it.
 */

main:
    cld

    # Reset segments
    cli
    xor ax, ax
    mov ds, ax
    mov ss, ax
    mov sp, offset start # Stack grows downwards from load address.
    sti

    # Get correct disk geometry
    push dx
    push es
    mov ah, 8
    int 0x13
    and cx, 0x3f
    mov sectorsPerTrack, cx
    mov al, cl
    movzx dx, dh
    add dx, 1
    mov numHeads, dx
    pop es
    pop dx

    # Reset disk system
    mov byte ptr bootDrive, dl # Save the drive BIOS booted from
    call resetDrive

    # Calculate FAT12 root directory info
    call loadRootInfo

    # Find first cluster of second stage file
    mov ax, LOAD_SEGMENT
    mov es, ax
    mov si, offset fileName
    call findFile
    push ax # save file cluster

    # Load FAT table into memory
    mov ax, FAT_SEGMENT
    mov es, ax
    call readFAT

    # Load second stage file into memory
    mov ax, LOAD_SEGMENT
    mov es, ax
    mov ax, FAT_SEGMENT
    pop cx # restore found file cluster
    call readFile

    jmp LOAD_SEGMENT:0
