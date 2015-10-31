.intel_syntax noprefix
.code16

.data
sector:    .byte 0
track:     .byte 0
head:      .byte 0
diskError: .string "DE"

MAX_READ_TRIES = 4

.text

/**
 * Reset drive specified by dl.
 */
.global resetDrive
resetDrive:
    xor ax, ax
    int 0x13
    jc diskFailure
    ret

/**
 * Convert LBA (logical block address) at ax to CHS (Cylinder-head-sector) address
 *
 * Conversion method:
 *     sector = (logical sector % sectors per track) + 1
 *     track  = (logical sector / sectors per track) / number of heads
 *     head   = (logical sector / sectors per track) % number of heads
 */
lba2chs:
    xor dx, dx                    # zero extend LBA (at ax) to dx:ax to prepare for division
    div word ptr sectorsPerTrack  # divide ax by sectorsPerTrack. Now ax = lba / spt and dx = lba % spt.
    inc dx                        # add 1 to remainder dx (since it's the sector)
    mov sector, dl

    xor dx, dx                    # zero extend quotient (lba / spt) at ax to dx:ax to prepare for division
    divw word ptr numHeads        # divide ax by numHeads. Now ax = (lba / spt) / numHeads and dx = (lba / spt) % numHeads
    mov head, dl
    mov track, al

    ret

/**
 * Read sector with LBA specified by ax into buffer at es:bx.
 * ax has the same value after this function. cx is modified.
 */
.global readSector
readSector:
    xor cx, cx # number of tries = 0

read:
    push ax # ax = LBA
    push cx # cx = number of tries

    call lba2chs

    mov ah, 2         # int 13h, ah=2: BIOS read sector
    mov al, 1         # al = sector count. read one sector
    mov cl, sector    # cl = sector
    mov ch, track     # ch = track (cylinder)
    mov dh, head      # dh = head
    mov dl, bootDrive # dl = drive num. read from boot drive
    int 0x13

    jnc success

    # try again..
    pop cx
    inc cx
    cmp cx, MAX_READ_TRIES
    je diskFailure # total failure

    # reset drive and try again
    call resetDrive
    pop ax
    jmp read

success:
    pop cx
    pop ax
    ret

/**
 * Read n (count specified by cx) sectors with LBA specified by ax into buffer at es:bx.
 * ax, cx and bx are modified by this function.
 *
 * The function increments es:bx by the number of bytes read.
 */
.global readSectors
readSectors:
    push cx
    call readSector
    inc ax
    add bx, sectorSize
    jc increaseSegment
    jmp nextSector

increaseSegment: # if bx overflows, increase es so that es:bx points to the next memory address
    mov cx, es
    add ch, 0x10 # es += 0x1000
    mov es, cx

nextSector:
    pop cx
    loopnz readSectors
    ret

/**
 * Disk failure - display message and reboot
 */
diskFailure:
    mov si, offset diskError
    call puts
    call reboot
