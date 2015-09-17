.intel_syntax noprefix
.code16

.data
pressKey: .string "Press any key to reboot.."

.text

/**
 * Print null-terminated string at ds:si.
 */
.global puts
puts:
    mov ah, 0x0e

putchar:
    lodsb
    cmp al, 0
    je end

    int 0x10
    jmp putchar

end:
    ret

/**
 * Wait for key.
 * ASCII value stored in al.
 */
.global getchar
getchar:
    xor ax, ax
    int 0x16
    ret

/**
 * Reboot system.
 */
.global reboot
reboot:
    mov si, offset pressKey
    call puts
    call getchar

    jmp 0xffff:0
