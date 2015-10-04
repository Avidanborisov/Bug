.intel_syntax noprefix
.code16

CODE_SELECTOR = 0x08
DATA_SELECTOR = 0x10

/**
 * The .entry section is the first section in the final kernel binary. It's just
 * a stub to the entry point that is guaranteed to be the first code in the binary
 */
.section .entry
jmp entry

/**
 * Entry point to the kernel from 16-bit land bootloader far jump.
 * Now switch to protected mode and jump to main kernel code (32-bit)
 */
.text
.global entry
entry:
    # Disable interrupts
    cli

    # Enable A20 gate (if disabled) using the fast method
    in al, 0x92
    test al, 2
    jnz a20enabled
    or al, 2
    and al, 0xfe
    out 0x92, al

a20enabled:
    # Load bootstrap GDT, required in order to switch to protected mode
    lgdt gdtSelector

    # Enable protected mode by setting the first bit of
    # cr0 to 1
    mov eax, cr0
    or al, 1
    mov cr0, eax

    # Transition to protected mode is done by a long jump
    # that reloads cs and eip. cs now holds the offset of the
    # gdt bootstrap code descriptor.
    jmp CODE_SELECTOR:protectedMode

.code32
protectedMode:
    # We're now in 32-bit protected mode!
    # Set all additional segment selectors to the kernel data segment
    mov ax, DATA_SELECTOR
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov esp, offset entry # the stack grows downwords from the starting address of the kernel
    call _init
    call kmain

spin: # infinite loop
    hlt
    jmp spin

.section .rodata

/**
 * The bootstrap GDT (global descriptor table) used to enter protected mode.
 * The layout of this table will be described in gdt.hpp
 */
gdt:
    # null descriptor
    .int 0, 0

    # kernel code descriptor (base=0, limit=0x000fffff, granularity is 1KB, ring 0, executable)
    # offset = 0x08 from gdt (CODE_SELECTOR)
    .word 0xffff # limit low
    .word 0      # base low
    .byte 0      # base middle
    .byte 0x9a   # access byte
    .byte 0xcf   # flags byte and limit high
    .byte 0      # base high

    # kernel data descriptor (base=0, limit=0x000fffff, granularity is 1KB, ring 0, not executable)
    # offset = 0x10 from gdt (DATA_SELECTOR)
    .word 0xffff # limit low
    .word 0      # base low
    .byte 0      # base middle
    .byte 0x92   # access byte
    .byte 0xcf   # flags byte and limit high
    .byte 0      # base high

gdtSelector:
    .word (gdtSelector - gdt - 1) # sizeof gdt - 1
    .long gdt                     # address of gdt
