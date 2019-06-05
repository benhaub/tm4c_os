/******************************************************************************
 * Authour	:	Ben Haubrich																										*
 * File			: link.ld																													*
 * Synopsis	:	linkerscript for tm4c_os																				*
 * Date			: May 16th, 2019																									*
 *****************************************************************************/
/* Full Description of an output section:
	section [address] [(type)] :
	[AT(lma)]
	[ALIGN(section_algin)]
	[SUBALIGN(subsection_align)]
	[constraint]
	{
		output-section-command
		output-section-command
		...
	} [>region] [AT>lma_region] : [:phdr :phdr ...] [=fillexp]
*/

MEMORY
{
	/* TODO: Flash should be further divided into kernel and user. */
	FLASH (wx) : ORIGIN = 0x00000000, LENGTH = 0x00040000 /* 256KB FLASH
	/* TODO: Maybe add the allocatable flag sto sram for malloc()? */
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

/*
 * Here's a quick walkthrough. We'll use the .text section as an example:
 * First we say that the output section .text will be loaded into 0x00000000.
 * This is because we want to put the vector table there according to where
 * the boot loader searches for it (see Pg.214 of the datasheet). Then we begin
 * to arrange the .text section. The .text section is generally used for data
 * that is permanent and not going to be changed (See Pg.32 of the GNU
 * assembler docs). We place the named section .intvecs from vectors.S in first
 * The location count is autmatically incremented now to be just at the end of
 * intvecs (Pg.39 GNU linker docs). Then we have all sections that are .text
 * placed in afeter. Then the linker is told to place these into the
 * memory region FLASH. Essentially what we are doing is treating the FLASH as
 * disk, and SRAM as RAM
 */
