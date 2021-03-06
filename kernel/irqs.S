.intel_syntax noprefix

.macro irq num
.global irq\num
irq\num:
    cli
    push 0
    push \num
    jmp commonTarget
.endm

irq  0
irq  1
irq  2
irq  3
irq  4
irq  5
irq  6
irq  7
irq  8
irq  9
irq 10
irq 11
irq 12
irq 13
irq 14
irq 15

commonTarget:
    call Context_save_before_handler
    call IRQ_commonHandler
    call Context_save_after_handler
