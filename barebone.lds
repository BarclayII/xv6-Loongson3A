OUTPUT_FORMAT("elf64-tradlittlemips")
OUTPUT_ARCH(mips)
ENTRY(_start)

SECTIONS
{
  .text : {
        *(.text)
	*(.rodata)
        }

  .reginfo : { *(.reginfo) }

  .data : {
	*(.data)
	}

  .bss  : {
	*(.dynbss)
	*(.bss)
  }
}
