
	list p=16f84
	radix dec

CONST	equ	2499

	movlw	high CONST
	movlw	CONST >> 8 & 0xFF

; HIGH
	cblock	0x10
	table_data: 1
	endc

	data	HIGH table_code
	data	HIGH table_data

table_code
	end
