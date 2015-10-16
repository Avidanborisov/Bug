.intel_syntax noprefix

retAddr: .int 0

.global context_save
context_save:
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

.global context_restore
context_restore:
    pop dword ptr retAddr

    add esp, 4 # pop Context::Registers pointer away

    pop eax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    popa # pop edi, esi, ebp, esp, ebx, edx, ecx, eax

    jmp dword ptr retAddr
