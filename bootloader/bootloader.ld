OUTPUT_FORMAT("binary")
ENTRY("mbrStart")

SECTIONS {
    .text 0x7c00 : {
        *(.text)
    }
}
