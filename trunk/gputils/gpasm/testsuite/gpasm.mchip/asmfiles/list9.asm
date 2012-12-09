; LIST9.ASM
;     This file tests the fixed and free format parser selection.

	LIST	P=16C71
	LIST	FIXED

	EXPAND			; directives
EXPAND

Label1	nop			; labels
;;;; Begin: Changed in gputils
;;;;   Label2	nop
Label2	nop
;;;; End: Changed in gputils
Macro1	MACRO			; macro definitions
	addlw	1
	ENDM

;;;; Begin: Changed in gputils
;;;;   Macro2	MACRO
Macro2	MACRO
;;;; End: Changed in gputils
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
;;;; Begin: Changed in gputils
;;;;   Label2a	nop
Label2a	nop
;;;; End: Changed in gputils
Macro1a	MACRO			; macro definitions
	addlw	1
	ENDM

;;;; Begin: Changed in gputils
;;;;   Macro2a	MACRO
Macro2a	MACRO
;;;; End: Changed in gputils
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
