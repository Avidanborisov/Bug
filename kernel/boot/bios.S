.intel_syntax noprefix
.code16

.global BIOS_interrupt
BIOS_interrupt:
    push edx
    push edi
    push esi

    mov eax, [esp + 20]
    mov intNum, al

    mov eax, [esp + 24]
    mov ebx, [esp + 28]
    mov ecx, [esp + 32]
    mov edx, [esp + 36]
    mov edi, [esp + 40]

    .byte 0xcd # INT opcode
intNum:
    .byte 0

    mov esi, [esp + 16]
    mov [esi], eax
    mov [esi + 4], ebx
    mov [esi + 8], ecx
    mov [esi + 12], edx
    mov [esi + 16], edi

    pop esi
    pop edi
    pop edx

    ret 4
