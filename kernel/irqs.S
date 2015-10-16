.intel_syntax noprefix

.macro create_irq num
.global irq\num
irq\num:
    cli
    push 0
    push \num
    jmp commonTarget
.endm

create_irq  0
create_irq  1
create_irq  2
create_irq  3
create_irq  4
create_irq  5
create_irq  6
create_irq  7
create_irq  8
create_irq  9
create_irq 10
create_irq 11
create_irq 12
create_irq 13
create_irq 14
create_irq 15

commonTarget:
    call context_save
    call irq_common_handler
    call context_restore

    add esp, 8 # two arguments were pushed on the stack before calling this function
    iret # pops cs, eip, eflags, ss, and esp and return from interrupt

