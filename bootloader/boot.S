.intel_syntax noprefix
.code16

.text
.global mbrStart

mbrStart:
    jmp 0:main # enforce CS = 0 because some BIOS' load the MBR to 0x7c0:0 instead of 0:0x7c00

# Program data
welcomeMsg: .string "Welcome to Bug OS!\r\n"
bootDrive:  .byte 0
diskError:  .string "Disk Error!"
pressKey:   .string "Press any key to reboot"

/**
 * The entry point for the bootloader code
 */
main:
    cld
    mov bootDrive, dl # Save the drive BIOS booted from

    cli
    mov ax, 0
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov ss, ax
    mov sp, 0x7c00 # Stack grows downwards from 0x7c00 (load address)
    sti

    mov si, offset welcomeMsg
    call print

    # Reset disk system
    mov dl, bootDrive
    mov ax, 0
    int 0x13
    jc bootFailure

    call reboot

bootFailure:
    mov si, diskError
    call print
    call reboot

/**
 * Print null-terminated string at ds:si
 */
print:
    lodsb
    cmp al, 0
    je printEnd

    mov ah, 0x0e
    int 0x10

    jmp print

printEnd:
    ret

/**
 * Reboot system
 */
reboot:
    mov si, offset pressKey
    call print
    mov ax, 0
    int 0x16

    jmp 0xffff:0

. = mbrStart + 510
bootMagic: .word 0xaa55
