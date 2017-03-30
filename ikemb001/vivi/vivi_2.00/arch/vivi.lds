SECTIONS { 
  . = 0x33f00000;
  .text          : { *(.text) }
  .data ALIGN(4) : { *(.data) }
  .bss ALIGN(4)  : { *(.bss)  *(COMMON) }
} 
