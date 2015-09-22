ENTRY(entry)

SECTIONS {
    . = 0x6000;

    .text : {
        *(.text)
    }
}
