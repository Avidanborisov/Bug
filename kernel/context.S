.intel_syntax noprefix

retAddr: .int 0

.global Context_save
Context_save:
    pop dword ptr retAddr

    pusha # push edi, esi, ebp, esp, ebx, edx, ecx, eax

    mov eax, ds
    push eax # save the data segment descriptor

    mov ax, GDT_DATA_SELECTOR # load the kernel data segment selector
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push esp # Context::Registers* regs = esp. push regs.

    jmp dword ptr retAddr

.global Context_restore
Context_restore:
    pop dword ptr retAddr

    add esp, 4 # pop Context::Registers pointer away

    pop eax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    popa # pop edi, esi, ebp, esp, ebx, edx, ecx, eax

    jmp dword ptr retAddr
