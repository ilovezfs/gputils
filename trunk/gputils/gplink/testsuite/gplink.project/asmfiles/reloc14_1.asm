	processor 16f877a

	extern	c3, d3, d4, d5, code2
	global	c1, c2

opf7	macro i
	i	0
	i	07F
	i	d1
	i	d2
	i	d3
	i	d4
	i	d1-d1b
	i	d1b-d1
	i	d1+1
	i	d2-1
	i	d3+2
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
	i	d3
	i	d3,F
	i	d3,W
	i	d1-d1b
	i	d1b-d1
	i	d1+1
	i	d2-1
	i	d3+2
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
	i	d3,0
	i	d3,7
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
	i	low c3
	i	high c3
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
	i	c3
	i	$
	i	$+1
	i	$-1
	endm


.data1	udata 0x20
	res 1
d1	res 1
d1b

.code1	code 0x20
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
	banksel	d3
	banksel	d4
	banksel	d5

	bankisel d1
	bankisel d2
	bankisel d3
	bankisel d4
	bankisel d5

	pagesel	c1
	pagesel c2
	pagesel c3
	pagesel $
	pagesel $+1

	DW	c1,c2,c3

c2

.data2	idata 0x28
	db 1
d2	db $,3
	dw $-1
	dw c3

	;; Test symbol values when merging code sections
	code
	pagesel code1
code1	dw code1, code2

	END
