/* ld template */
MEMORY
{
        FLASH (xr): ORIGIN = 0x00020000, LENGTH = 64K
     RAM (xrw) : ORIGIN = 0x68001400, LENGTH = 11K
}

SECTIONS
{
    .isr_vector :
    {
    _stext = .;
        KEEP(*(.header))            /* Startup code */
    } >FLASH

     /* the program code is stored in the .text section, which goes to Flash */
    .text :
    {
        . = ALIGN(4);        
        *(.text)                   /* remaining code */
        *(.text.*)                 /* remaining code */
    } >FLASH
 	.init :
    {
    __NDK_initcall_start = .;
        KEEP(*(.NDK.init))
    __NDK_initcall_end = .;
    } >FLASH
    
    .ARM.extab : {
		*(.ARM.extab*)
	} >FLASH
	.ARM.exidx : {
		__exidx_start = .;
		*(.ARM.exidx*)
		__exidx_end = .;
	} >FLASH
        
    .rodata :
    {
        *(.rodata)                 /* read-only data (constants) */
        *(.rodata*)
        *(.drivers)
        *(.glue_7) 
        *(.glue_7t)
        . = ALIGN(4);
        _etext = .;
        _sidata = _etext;
    } >FLASH
	
	.data  : AT ( _sidata )
    {
        /* This is used by the startup in order to initialize the .data secion */
        . = ALIGN(4);
        _sdata = . ;
        *(.data)
        *(.data.*)
        . = ALIGN(4);
        /* This is used by the startup in order to initialize the .data secion */
     _edata = . ;
    } >RAM

    /* This is the initialized data section
    The program executes knowing that the data is in the RAM
    but the loader puts the initial values in the FLASH (inidata).
    It is one task of the startup to copy the initial values from FLASH to RAM. */
    .bss (NOLOAD) :
    {
        _sbss = .; /* Used by the startup in order to initialize the .bss section */
         *(.bss)
         *(.bss.*)
         *(COMMON)
        . = ALIGN(4);
        _ebss = . ; /* Used by the startup in order to initialize the .bss section */
    } >RAM
}
_binlen = _etext - _stext + _edata - _sdata;