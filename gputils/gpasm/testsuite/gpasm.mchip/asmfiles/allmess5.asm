;ALLMESS5.ASM
;	Tests generation of warnings.

	list	p=16c71, r=dec, fixed
	
#undefine NotDefined			; Warning 201 Symbol not previously defined

	addlw	0x123			; Warning 202 Argument out of range.  Least...

nop					; Warning 203 Found opcode in column 1.

b	0				; Warning 204 Found pseudo-op in column 1.

expand					; Warning 205 Found directive in column 1.

Mac1	macro
	nop
	endm

Mac1					; Warning 206 Found call to macro in column 1.

	Label1				; Warning 207 Found label after column 1.

This_Label_Is_Just_A_Bit_Too_Long	; Warning 208 Label truncated at 32 characters.

					; Warning 209 Missing quote
	data	"123

	cblock	
	   Val1, Val2,			; Warning 210 Extra ","
	endc

	nop	1			; Warning 211 Extra arguments on the line.

	fill	(goto 0, 5		; Warning 214 Unmatched (

	radix	abc			; Warning 218 Expected DEC, OCT...

	__MAXRAM	0x2f
	__BADRAM	20
	movwf		20		; Warning 219 Invalid RAM location specified

	org	0x1000
	nop				; Warning 220 Address exceeds maximum range...
	org	0x10

	errorlevel	-999		; Warning 221 Invalid message number

	errorlevel	-101		; Warning 222 Error messages cannot be disabled

	processor	16c71		; Warning 223 Redefining processor

	tris	6			; Warning 224 Use of this instruction is not recommended
	option				; Warning 224 Use of this instruction is not recommended

;***** Test for SSR 2074

MyMacro	macro
	nop
	endm

	call	MyMacro			; Warning 225 Invalid label in operand
	data	MyMacro			; Warning 225 Invalid label in operand

;***** End Test

	if Label1
	end				; Warning 212 Expected...


