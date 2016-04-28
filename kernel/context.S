.intel_syntax noprefix

retAddr: .int 0

.macro save_registers
    pusha # save edi, esi, ebp, useless, ebx, edx, ecx, eax

    mov eax, ds
    push eax # save the data segment selector
.endm

.global Context_save_full #(esi = resume address, edi = additional flags)
Context_save_full:
    pop dword ptr retAddr

    mov eax, ss
    push eax                    # save ss
    mov eax, esp
    push eax                    # save esp
    pushfd                      # save eflags (with additional flags specified)
    or dword ptr [esp], edi
    mov eax, cs
    push eax                    # save cs
    push esi                    # save resume address
    push 0                      # save fake errorCode
    push 0                      # save fake intNum

    save_registers

    jmp dword ptr retAddr

.global Context_restore
Context_restore:
    pop dword ptr retAddr

    pop eax    # load ds
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    popa       # load edi, esi, ebp, useless, ebx, edx, ecx, eax

    add esp, 8 # skip intNum, errorCode
    iret       # load eip, cs, eflags, esp, ss

.global Context_save_before_handler
Context_save_before_handler:
    pop dword ptr retAddr

    save_registers

    mov ax, GDT_DATA_SELECTOR # load the kernel data segment selector
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push esp # Context::Registers* regs = esp. push regs.*/

    jmp dword ptr retAddr

.global Context_save_after_handler
Context_save_after_handler:
    pop dword ptr retAddr

    add esp, 4 # pop Context::Registers pointer away
    call Context_restore
