; LINNO1.ASM

; This file tests line number entry for COFF files.  Macro generated
; lines should point to the invokation line, not the definition line.

	list	p=16c61

Mac1	macro
	clrw
	endm

Mac2	macro	Times
Loop	= 0
	while Loop < Times
	   addlw	1
Loop ++
	endw
	endm


	code

	nop
	Mac1
	nop
	Mac2	4
	nop
	Mac1
	nop
	Mac2	9
	nop
	Mac1
	nop

	end
