/******************************************************************************
 * Authour	:	Ben Haubrich																										*
 * File			: link.ld																													*
 * Synopsis	:	linkerscript for tm4c_os																				*
 * Date			: May 16th, 2019																									*
 *****************************************************************************/
/* Please see "The GNU linker" documentation for information on linker */
/* scripts. */

MEMORY
{
	FLASH (wx) : ORIGIN = 0x00000000, LENGTH = 0x00040000 /* 256KB FLASH */
	SRAM	(wx)	: ORIGIN = 0x20000000, LENGTH = 0x00008000 /* 32KB SRAM */
}

SECTIONS
{
	.text 0x00000000 :
	{
		_text = .;
		KEEP(*(.intvecs))
		*(.text)
		_etext = .;
	} >FLASH
	
	.text.smain : 
	{
		_text.smain = .;
		KEEP(*(.text.smain))
		_etext.smain = .;
	} >FLASH
	
	.data : 
	{
		_data = .;
		*(.data)
		_edata = .;
	} >SRAM
	
	.bss :
	{
		_bss = .;
		*(.bss)
		*(COMMON)
		_ebss = .;
	} >SRAM

	.stack :
	{
		_stack = .;
		KEEP(*(.stack))
		_estack = .;
	} >SRAM
	
	/* 
	 * This section is put in automatically by the arm compiler. It's asking for
	 * some c library exception handling code that I don't want to include.
	 */
	/DISCARD/ :
	{
		*(.ARM.exidx)
	}
}
