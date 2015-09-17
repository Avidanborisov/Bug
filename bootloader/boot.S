.intel_syntax noprefix
.code16

FAT_SEGMENT  = 0x0ee0 # segment to load the FAT table to
LOAD_SEGMENT = 0x1000 # segment to load the second stage binary to

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
    fileName:            .ascii  "KERNEL     " # Volume label (11 chars max, padded with zeros). Here it's the second stage file name.
    fsType:              .ascii  "FAT12   "    # File system type (8 chars max, padded with zeros)

.global bootDrive, sectorsPerTrack, numHeads, sectorSize, numRootEntries, \
        numFATs, sectorsPerFat, numHiddenSectors, numReservedSectors, sectorsPerFAT

stub:
    jmp 0:main # enforce CS = 0 because some BIOS' load the MBR to 0x7c0:0 instead of 0:0x7c00

/**
 * The main bootloader code. Loads a second stage binary image to LOAD_SEGMENT
 * and jumps to it.
 */
main:
    cld
    mov bootDrive, dl # Save the drive BIOS booted from

    cli
    xor ax, ax
    mov ds, ax
    mov ss, ax
    mov sp, 0x7c00 # Stack grows downwards from 0x7c00 (load address)
    sti

    # Reset disk system
    mov dl, bootDrive
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
