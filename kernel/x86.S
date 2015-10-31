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

.global x86_lgdt
x86_lgdt:
    mov eax, [esp + 4] # eax = address of gdt pointer
    lgdt [eax]         # load gdt using the gdt pointer eax points at
    ret

.global x86_lidt
x86_lidt:
    mov eax, [esp + 4] # eax = address of idt pointer
    lidt [eax]         # load idt using the idt pointer eax points at
    ret

.global x86_sti
x86_sti:
    sti
    ret

.global x86_cli
x86_cli:
    cli
    ret

.global x86_interrupt
x86_interrupt:
    mov eax, [esp + 4]
    mov intNum, al

    .byte 0xcd # INT opcode
intNum:
    .byte 0

    ret

.global x86_cs
x86_cs:

.global x86_set_cs
x86_set_cs:
    mov eax, [esp + 8] # 1st arg is this pointer
    push eax
    push offset reload
    retf
reload:
    ret

.global x86_get_cs
x86_get_cs:
    mov eax, cs
    ret

.macro createRegisterProperty reg
.global x86_\reg
x86_\reg:

.global x86_set_\reg
x86_set_\reg:
    mov eax, [esp + 8] # 1st arg is this pointer
    mov \reg, eax
    ret

.global x86_get_\reg
x86_get_\reg:
    mov eax, \reg
    ret
.endm

createRegisterProperty ds
createRegisterProperty es
createRegisterProperty fs
createRegisterProperty gs
createRegisterProperty ss
createRegisterProperty cr0
createRegisterProperty cr2
createRegisterProperty cr3
