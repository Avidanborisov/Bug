.intel_syntax noprefix
.code16

.global entry
entry:
    jmp 0:second

second:
    cli
    mov ax, 0
    mov ds, ax
    sti

    call reboot

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

getchar:
    xor ax, ax
    int 0x16
    ret

reboot:
    mov si, offset pressKey
    call puts
    call getchar

    jmp 0xffff:0

pressKey: .string "Stage 2! Press any key to reboot!"
