; EXPAND1.ASM

; This file is for testing macro expansion.

	LIST	P=16C54

MAC1	MACRO	
	NOP
	ENDM


	MAC1

	EXPAND		1		; Illegal parameter
	
	MAC1

	NOEXPAND	1		; Illegal parameter

	MAC1

Label1	EXPAND				; Illegal label

	MAC1

Label2	NOEXPAND			; Illegal label

	MAC1

	END	
