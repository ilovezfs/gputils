; LIST9.ASM
;     This file tests the fixed and free format parser selection.

	LIST	P=16C71
	LIST	FIXED

	EXPAND			; directives
EXPAND

Label1	nop			; labels
   Label2	nop

Macro1	MACRO			; macro definitions
	addlw	1
	ENDM

   Macro2	MACRO
	addlw	1
ENDM

	Macro1			; macro calls
Macro1

	Macro2
Macro2

	addlw	1		; opcodes
addlw	1

	b	0		; pseudo-ops
b	0

;**************************************************************************

	LIST	FREE

	EXPAND			; directives
EXPAND

Label1a	nop			; labels
   Label2a	nop

Macro1a	MACRO			; macro definitions
	addlw	1
	ENDM

   Macro2a	MACRO
	addlw	1
ENDM

	Macro1a			; macro calls
Macro1a

	Macro2a
Macro2a

	addlw	1		; opcodes
addlw	1

	b	0		; pseudo-ops
b	0

	end
