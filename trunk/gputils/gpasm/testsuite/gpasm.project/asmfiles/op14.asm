	processor 16f877a

opf7	macro i
	i	0
	i	07F
	i	d1
	i	d2
	i	d1-d1b
	i	d1b-d1
	i	d1+1
	i	d2-1
	endm

opwf7	macro i
	i	0
	i	0,F
	i	0,W
	i	07F
	i	07F,F
	i	07F,W
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

b7	macro i
	i	0,0
	i	0,7
	i	07F,0
	i	07F,7
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
	i	07FF
	i	c1
	i	c2
	i	$
	i	$+1
	i	$-1
	endm


	cblock 0x21
	d1
	d1b
	endc

	org 0x20
	nop
c1

	lit8	ADDLW
	opwf7	ADDWF
	lit8	ANDLW
	opwf7	ANDWF
	b7	BCF
	b7	BSF
	b7	BTFSC
	b7	BTFSS
	lit11	CALL
	opf7	CLRF
	CLRW
	CLRWDT     
	opwf7	COMF
	opwf7	DECF
	opwf7	DECFSZ
	lit11	GOTO
	opwf7	INCF
	opwf7	INCFSZ
	lit8	IORLW
	opwf7	IORWF
	opwf7	MOVF
	lit8	MOVLW
	opf7	MOVWF
	NOP
	OPTION
	RETFIE
	lit8	RETLW
	RETURN
	opwf7	RLF
	opwf7	RRF
	SLEEP
	lit8	SUBLW
	opwf7	SUBWF
	opwf7	SWAPF
	lit8	XORLW
	opwf7	XORWF

	banksel	d1
	banksel	d2

	pagesel	c1
	pagesel c2

	DW	c1,c2

c2

	cblock 0x29
	d2
	endc

	END
