.intel_syntax noprefix

.macro create_isr num
.global isr\num
isr\num:
    cli
    push \num
    jmp commonTarget
.endm

.macro create_isr_no_error num
.global isr\num
isr\num:
    cli
    push 0 // dummy error code
    push \num
    jmp commonTarget
.endm

create_isr_no_error  0
create_isr_no_error  1
create_isr_no_error  2
create_isr_no_error  3
create_isr_no_error  4
create_isr_no_error  5
create_isr_no_error  6
create_isr_no_error  7
create_isr           8
create_isr_no_error  9
create_isr          10
create_isr          11
create_isr          12
create_isr          13
create_isr          14
create_isr          15
create_isr_no_error 16
create_isr_no_error 17
create_isr_no_error 18
create_isr_no_error 19
create_isr_no_error 20
create_isr_no_error 21
create_isr_no_error 22
create_isr_no_error 23
create_isr_no_error 24
create_isr_no_error 25
create_isr_no_error 26
create_isr_no_error 27
create_isr_no_error 28
create_isr_no_error 29
create_isr_no_error 30
create_isr_no_error 31

commonTarget:
    call context_save
    call isr_common_handler
    call context_restore

    add esp, 8 # two arguments were pushed on the stack before calling this function
    iret # pops cs, eip, eflags, ss, and esp and return from interrupt

