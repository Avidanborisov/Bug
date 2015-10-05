.intel_syntax noprefix

.global x86_out
x86_out:
    mov al, [esp + 8]
    mov dx, [esp + 4]
    out dx, al
    ret

.global x86_inb
x86_inb:
    mov dx, [esp + 4]
    in al, dx
    ret

.global x86_inw
x86_inw:
    mov dx, [esp + 4]
    in ax, dx
    ret
