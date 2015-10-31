ENTRY(entry)

SECTIONS {
    /**
     * This is the physical address the kernel will be loaded to
     * it must match the value of LOAD_SEGMENT in the bootloader
     */
    . = 0x9000;

    .text : {
        *(.entry)
        *(.text)
    }

    .rodata : {
        *(.rodata)
    }

    .data : {
        *(.data)
    }

    .bss : {
        *(COMMON)
        *(.bss)
    }

    kernelEnd = .;
}
