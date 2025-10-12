OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_start)

SECTIONS
{
    /* Exception vector table and early boot code - must be at 0x0 */
    . = 0x0;
    .init :
    {
        *(.init)           /* Exception vectors and reset code */
        *(.boot_early)     /* Early boot functions like led_main */
        . = ALIGN(4);
    }

    /* Main program code */
    . = ALIGN(4);
    .text : 
    {
        *(.text)
        *(.gnu.linkonce.t*)
        
        /* Section information for finsh shell */
        . = ALIGN(4);
        __fsymtab_start = .;
        KEEP(*(FSymTab))
        __fsymtab_end = .;
        . = ALIGN(4);
        __vsymtab_start = .;
        KEEP(*(VSymTab))
        __vsymtab_end = .;
        . = ALIGN(4);   

        /* RT-Thread initialization functions */
        . = ALIGN(4);
        __rt_init_start = .;
        KEEP(*(SORT(.rti_fn*)))
        __rt_init_end = .;
        . = ALIGN(4);

        /* Section information for modules */
        . = ALIGN(4);
        __rtmsymtab_start = .;
        KEEP(*(RTMSymTab))
        __rtmsymtab_end = .;
    }

    /* Read-only data */
    . = ALIGN(4);
    .rodata : 
    { 
        *(.rodata) 
        *(.rodata.*) 
        *(.gnu.linkonce.r*) 
        *(.eh_frame) 
    }

    /* Constructors */
    . = ALIGN(4);
    .ctors :
    {
        PROVIDE(__ctors_start__ = .);
        KEEP(*(SORT(.ctors.*)))
        KEEP(*(.ctors))
        PROVIDE(__ctors_end__ = .);
    }

    /* Destructors */
    .dtors :
    {
        PROVIDE(__dtors_start__ = .);
        KEEP(*(SORT(.dtors.*)))
        KEEP(*(.dtors))
        PROVIDE(__dtors_end__ = .);
    }

    /* Initialized data */
    . = ALIGN(4);
    .data :
    {
        *(.data)
        *(.data.*)
        *(.gnu.linkonce.d*)
    }

    /* No-BSS section */
    . = ALIGN(4);
    .nobss : 
    { 
        *(.nobss) 
    }

    /* BSS section - runtime in SDRAM */
    . = 0x30200000;
    . = ALIGN(4);
    __bss_start = .;
    .bss : 
    { 
        *(.bss) 
    }
    __bss_end = .;

    /* Debug sections - not loaded into memory */
    .stab 0 : 
    { 
        *(.stab) 
    }
    .stabstr 0 : 
    { 
        *(.stabstr) 
    }
    .stab.excl 0 : 
    { 
        *(.stab.excl) 
    }
    .stab.exclstr 0 : 
    { 
        *(.stab.exclstr) 
    }
    .stab.index 0 : 
    { 
        *(.stab.index) 
    }
    .stab.indexstr 0 : 
    { 
        *(.stab.indexstr) 
    }
    .comment 0 : 
    { 
        *(.comment) 
    }
    .debug_abbrev 0 : 
    { 
        *(.debug_abbrev) 
    }
    .debug_info 0 : 
    { 
        *(.debug_info) 
    }
    .debug_line 0 : 
    { 
        *(.debug_line) 
    }
    .debug_pubnames 0 : 
    { 
        *(.debug_pubnames) 
    }
    .debug_aranges 0 : 
    { 
        *(.debug_aranges) 
    }

    _end = .;
}
