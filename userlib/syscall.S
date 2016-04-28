.intel_syntax noprefix

.global syscall
syscall:
    push ebx

    mov eax, [esp + 8]
    mov ebx, [esp + 12]
    mov ecx, [esp + 16]
    mov edx, [esp + 18]
    int 80

    pop ebx
    ret
