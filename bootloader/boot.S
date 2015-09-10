.intel_syntax noprefix
.code16

.text
.global mbrStart

mbrStart:
    jmp codeStart
    msg: .string "Welcome to Bug OS!"

codeStart:
    mov ax, 0
    mov ds, ax
    mov es, ax
    mov ss, ax

    mov sp, 0x7c00 # Stack grows downwards from 0:0x7c00 (load address)

    mov si, offset msg
    call print
    jmp .

print:
    lodsb
    cmp al, 0
    je end
    mov ah, 0x0e
    int 0x10
    jmp print

end:
    ret

. = mbrStart + 510
bootMagic: .word 0xaa55
