/* ld template */
MEMORY
{
    RAM : ORIGIN = 0x400C000, LENGTH = 64K
    NOR : ORIGIN = 0x2F8000, LENGTH = 384K
}

REGION_ALIAS("REGION_TEXT", NOR);
REGION_ALIAS("REGION_RODATA", NOR);
REGION_ALIAS("REGION_DATA", RAM);
REGION_ALIAS("REGION_BSS", RAM);
REGION_ALIAS("REGION_ARMEXIDX", RAM);

SECTIONS
{
        .mod_init :
        {
                _stext = .;
                KEEP(*(.mod.header))
        } >REGION_TEXT
        .text :
        {
                . = ALIGN(4);
                *(.text)                   /* remaining code */
                *(.text.*)                 /* remaining code */
        } >REGION_TEXT
        .rodata :
        {
                *(.rodata)                 /* read-only data (constants) */
                *(.rodata*)
                *(.glue_7)                 /* ARM to Thumb,unused */
                *(.glue_7t)                /* Thumb to ARM,unused */
                . = ALIGN(4);
                _etext = .;
                /* This is used by the startup in order to initialize the .data secion */
                _sidata = _etext;
        } >REGION_RODATA
        .data  : AT ( _sidata )
        {
                . = ALIGN(4);
                /* This is used by the startup in order to initialize the .data secion */
                _sdata = . ;
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
        .ARM.exidx :
        {
        	. = ALIGN(4);
                 *(.exidx)
                 *(.exidx.*)	
        } >REGION_ARMEXIDX   
}
