OUTPUT_FORMAT("binary")
ENTRY("start")

SECTIONS {
    . = 0x7c00;

    .text : {
        *(.text);
    }

    .data : {
        *(.data);
    }

    /* Boot magic signature */
    .magic : AT(0x7dfe) {
        SHORT(0xaa55);
    }
}
