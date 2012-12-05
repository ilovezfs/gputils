	processor 16c58b

opf5	macro i
	i	0
	i	01F
	i	d1
	i	d2
	i	d1-d1b
	i	d1b-d1
	i	d1+1
	i	d2-1
	endm

opwf5	macro i
	i	0
	i	0,F
	i	0,W
	i	01F
	i	01F,F
	i	01F,W
	i	d1
	i	d1,F
	i	d1,W
	i	d2
	i	d2,F
	i	d2,W
	i	d1-d1b
	i	d1b-d1
	i	d1+1
	i	d2-1
	endm

b5	macro i
	i	0,0
	i	0,7
	i	01F,0
	i	01F,7
	i	d1,0
	i	d1,7
	i	d2,0
	i	d2,7
	endm

lit8	macro i
	i	0
	i	'A'
	i	"A"
	i	0FF
	i	low $
	i	high $
	i	low c1
	i	high c1
	i	low c2
	i	high c2
	i	$
	i	$+1
	i	$-1
	;; gpasm bugs prevent these
	;i	$-$
	;i	c1-$
	;i	$-c1
	;i	c2-$
	;i	$-c2
	i	d1-d1b
	i	d1b-d1
	endm

lit11	macro i
	i	0
	;; MPASM (correctly) gives error on these for a CALL
	;i	01FF
	i	0200
	;i	07FF
	i	c1
	i	c2
	i	$
	i	$+1
	i	$-1
	endm

p6	equ	6

	cblock 0x11
	d1
	d1b
	endc

	org 0x20
	nop
c1

	opwf5	ADDWF
	lit8	ANDLW
	opwf5	ANDWF
	b5	BCF
	b5	BSF
	b5	BTFSC
	b5	BTFSS
	lit11	CALL
	opf5	CLRF
	CLRW
	CLRWDT     
	opwf5	COMF
	opwf5	DECF
	opwf5	DECFSZ
	lit11	GOTO
c2
	opwf5	INCF
	opwf5	INCFSZ
	lit8	IORLW
	opwf5	IORWF
	opwf5	MOVF
	lit8	MOVLW
	opf5	MOVWF
	NOP
	OPTION
	lit8	RETLW
	RETURN
	opwf5	RLF
	opwf5	RRF
	SLEEP
	opwf5	SUBWF
	opwf5	SWAPF
	TRIS	6
	TRIS	7
	TRIS	p6
	TRIS	p7
	lit8	XORLW
	opwf5	XORWF

	banksel	d1
	banksel	d2

	pagesel	c1
	pagesel c2

	DW	c1,c2

	cblock 0x19
	d2
	endc

	cblock 7
	p7
	endc
	
	END
