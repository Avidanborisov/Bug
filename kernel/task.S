.intel_syntax noprefix

.global Task_init # void Task::init(int firstPid)
Task_init:
    mov eax, [esp + 4]
    push eax
    call Scheduler_getTaskCr3
    mov cr3, eax
    call Scheduler_getTaskContext
    mov esp, [eax]

    call Context_restore

.global Task_replace # void Task::replace(int currPid, int newPid)
Task_replace:
    mov ecx, [esp + 8] # currPid
    mov edx, [esp + 4] # oldPid
    push esi
    push edi
    cli

# prepare new data for current context
    lea esi, [resume] # make execution resume from 'resume'
    mov edi, 0x200    # eflags will have interrupts enabled
    call Context_save_full

# saved updated current context
    push ecx
    push edx
    call Scheduler_getTaskContext # get context of currPid
    pop edx
    pop ecx
    mov [eax], esp # update context to point at current stack top

# switch to cr3 of new context
    push edx
    push ecx
    call Scheduler_getTaskCr3 # get cr3 of newPid
    pop ecx
    pop edx
    mov cr3, eax

# switch to new context
    push edx
    push ecx
    call Scheduler_getTaskContext # get context of newPid
    pop ecx
    pop edx
    mov esp, [eax] # update stack to point at new context

    mov eax, [esp] # restore ds
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    call Context_restore

.global resume
resume: # location to continue execution from after iret
    # iret popped eflags, esp, ss. esp and ss need to be removed to
    # have return address at the top of the stack
    pop eax
    pop eax

    pop esi
    pop edi

    ret
