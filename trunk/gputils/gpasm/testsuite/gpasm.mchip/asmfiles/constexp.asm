; CONSTEXP.ASM

; This file is used to evaluate how constant expressions are parsed.

	LIST	P=17C44

Quoted	MACRO
	DATA	'1'
	DATA	','
	DATA	'('
	DATA	')'
	DATA	'\''
	DATA	'\"'
	DATA	A'1'
	DATA	a'1'
	DATA	B'100'
	DATA	b'100'
	DATA	D'100'
	DATA	d'100'
	DATA	H'100'
	DATA	h'100'
	DATA	O'100'
	DATA	o'100'
	DATA	Q'100'
	DATA	q'100'
	ENDM

Straight	MACRO
	DATA	1
	DATA	10
	DATA	100
	ENDM

Leading	MACRO
	DATA	.10
	DATA	.100
	DATA	0x10
	DATA	0x100
	ENDM

Trailing	MACRO
	DATA	100b
	DATA	100B
	DATA	100d
	DATA	100D
	DATA	100h
	DATA	100H
	DATA	100o
	DATA	100O
	DATA	100q
	DATA	100Q
	ENDM


	RADIX	HEX
	Quoted
	Straight
	Leading
	Trailing

	RADIX	DEC
	Quoted
	Straight
	Leading
	Trailing

	RADIX	OCT
	Quoted
	Straight
	Leading
	Trailing

	RADIX	HEX
	DATA	0B9
	DATA	'A'
	DATA	A'A'
	DATA	"ABC"
	DATA	"\n\123\t\321"

;***** Test for SSR 2060 **************************************************

	DATA	(D'90'-'(') * 2		; quoted paren inside paren
	DATA	(D'90'-"(") * 2		; quoted paren inside paren

;***** End Test ***********************************************************

;***** Test for SSR 2883 **************************************************

	DATA	-'='			; negative equals sign

;***** End Test ***********************************************************

	END
