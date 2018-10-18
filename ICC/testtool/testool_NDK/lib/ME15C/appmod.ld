/* ld template */
MEMORY
{
        RAM : ORIGIN = 0x0021a628, LENGTH = 58K
        NOR : ORIGIN = 0x00033000, LENGTH = 16K
}
REGION_ALIAS("REGION_TEXT", NOR);
REGION_ALIAS("REGION_RODATA", NOR);
REGION_ALIAS("REGION_DATA", RAM);
REGION_ALIAS("REGION_BSS", RAM);

SECTIONS
{
        .mod_init :
        {
                _stext = .;
                KEEP(*(.mod.header))
                _etext = .;
        } >REGION_TEXT
        .text : AT ( _etext )
        {
        		_sdata = .;
                . = ALIGN(4);
                *(.text)                   /* remaining code */
                *(.text.*)                 /* remaining code */
        } >REGION_DATA
        .rodata :
        {
                *(.rodata)                 /* read-only data (constants) */
                *(.rodata*)
                *(.glue_7)                 /* ARM to Thumb,unused */
                *(.glue_7t)                /* Thumb to ARM,unused */
                . = ALIGN(4);
                /* This is used by the startup in order to initialize the .data secion */
                
        } >REGION_DATA
        .data  :
        {
                . = ALIGN(4);
                /* This is used by the startup in order to initialize the .data secion */
                *(.data)
                *(.data.*)
                . = ALIGN(4);
                /* This is used by the startup in order to initialize the .data secion */
                _edata = . ;
        } >REGION_DATA
        .bss (NOLOAD) :
        {
                . = ALIGN(4);
                _sbss = .; /* Used by the startup in order to initialize the .bss section */
                 *(.bss)
                 *(.bss.*)
                 *(COMMON)
                . = ALIGN(4);
                _ebss = . ; /* Used by the startup in order to initialize the .bss section */
                _end = .;
        } >REGION_BSS
}
