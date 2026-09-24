ENTRY(moduleInit)
SECTIONS {
    . = MODULE_ADDRESS - 32;
    .module : {
        LONG(0x444f4d53) LONG(1) LONG(MODULE_REGION) LONG(MODULE_ADDRESS)
        LONG(_image_end - MODULE_ADDRESS) LONG(_memory_end - MODULE_ADDRESS)
        LONG(moduleInit - MODULE_ADDRESS) LONG(moduleUpdate - MODULE_ADDRESS)
    }
    .text : { *(.text .text.*) }
    .rodata ALIGN(16) : { *(.rodata .rodata.*) }
    .data ALIGN(16) : { *(.data .data.*) *(.sdata .sdata.*) . = ALIGN(16); }
    _image_end = .;
    .bss ALIGN(16) (NOLOAD) : { *(.bss .bss.*) *(.sbss .sbss.*) *(COMMON) }
    _memory_end = ALIGN(16);
    /DISCARD/ : { *(.reginfo) *(.MIPS.abiflags) *(.pdr) *(.comment) *(.gnu.attributes) }
}
