; EXPNDMAC.ASM

; This file is for testing macro expansion.

	LIST	P=16C71

MAC1	MACRO	
	NOP
	ENDM


	MAC1

	EXPAND				

	MAC1

	NOEXPAND

	MAC1

	EXPAND				

	MAC1

	NOEXPAND

	MAC1

;***** Test for SSR 1928

Mac1	macro
	nop
	Mac2
	nop
	endm

Mac2	macro
	addlw	1
	endm

Mac3	macro
	nop
	noexpand
	Mac4
	expand
	nop
	endm

Mac4	macro
	addlw	1
	endm

Mac5	macro
	nop
	Mac6
	nop
	endm

Mac6	macro
	noexpand
	addlw	1
	expand
	endm

	expand
	Mac1
	noexpand
	Mac1
	expand

	Mac3

	Mac5

;***** End Test

	END	
