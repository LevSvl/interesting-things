OUTPUT_ARCH( "riscv" )

SECTIONS
{
    .text : { *(.text) }
  
    .rodata : {
        *(.rodata)
        *(.rodata.*)
    }
    .data : { *(.data) }
  
    .bss : { *(.bss) }
    
    .dtb : {
        _dtb_start = .;
        *(.rodata.dtb)
        _dtb_end = .;
    }
  
    _end = .;
}
