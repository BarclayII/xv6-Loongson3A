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

	PROVIDE(__data__=.);

	.data : {
		*(.data)
	}

	PROVIDE(__edata__=.);

	.bss  : {
		*(.dynbss)
		*(.bss)
	}

	PROVIDE(__end__ = .);
}
