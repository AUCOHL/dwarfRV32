ENTRY(_start)

/* Define the physical memory */
/* One memory block of 4KB   */
/* 1st 3KB is called ram     */
/* last 1KB is called stack  */

MEMORY
{
    ram :
        ORIGIN = 0x00000000,
        LENGTH = 20K
    stack :
        ORIGIN = 20K
        LENGTH = 6K
}

SECTIONS
 {
 . = 0x00;

 /* text: test code section */
 .text :
 {
    *(.text)
 } > ram

 /* data: Initialized data segment */
 .data :
 {
    *(.data)
 } > ram

 /* Uintialized data */
  .bss :
  {
    *(.bss)
  } > ram
 /* End of uninitalized data segement */
 _end = .;
 }

PROVIDE(_endram = ORIGIN(stack));
PROVIDE(_fstack = ORIGIN(stack) + LENGTH(stack) - 4);
