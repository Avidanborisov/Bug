.intel_syntax noprefix

.macro isr num
.global isr\num
isr\num:
    cli
    push \num
    jmp commonTarget
.endm

.macro isrNoError num
.global isr\num
isr\num:
    cli
    push 0 # dummy error code
    push \num
    jmp commonTarget
.endm

isrNoError  0
isrNoError  1
isrNoError  2
isrNoError  3
isrNoError  4
isrNoError  5
isrNoError  6
isrNoError  7
isr         8
isrNoError  9
isr        10
isr        11
isr        12
isr        13
isr        14
isr        15
isrNoError 16
isrNoError 17
isrNoError 18
isrNoError 19
isrNoError 20
isrNoError 21
isrNoError 22
isrNoError 23
isrNoError 24
isrNoError 25
isrNoError 26
isrNoError 27
isrNoError 28
isrNoError 29
isrNoError 30
isrNoError 31

isrNoError 80 # syscall handler

commonTarget:
    call Context_save_before_handler
    call ISR_commonHandler
    call Context_save_after_handler
