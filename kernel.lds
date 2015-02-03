OUTPUT_FORMAT("elf64-tradlittlemips")
OUTPUT_ARCH(mips)
ENTRY(_start)

SECTIONS
{
  .text : {
        *(.text)
	*(.rodata)
        }

  __ex_table : {
	__begin___ex_table = .;
	*(__ex_table);
	__end___ex_table = .;
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
