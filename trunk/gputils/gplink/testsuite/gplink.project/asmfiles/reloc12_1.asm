	processor 12c509

	extern	c3, d3, d4, p6
	global	c1, c2

opf5	macro i
	i	0
	i	01F
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
	i	d3
	i	d3,F
	i	d3,W
	i	d1-d1b
	i	d1b-d1
	i	d1+1
	i	d2-1
	i	d3+2
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
	;; MPASM (correctly) gives error on these for a CALL
	;i	01FF
	i	0200
	;i	07FF
	i	c1
	i	c2
	i	c3
	i	$
	i	$+1
	i	$-1
	endm


.data1	udata 0x10
	res 1
d1	res 1
d1b

.code1	code 0x20
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
	banksel	d3

	pagesel	c1
	pagesel c2
	pagesel c3
	pagesel $
	pagesel $+1

	DW	c1,c2,c3

.data2	udata 0x18
	res 1
d2	res 1

.data0	udata 7
p7	res 1
	
	END
