; LOCAL2.ASM

; This file tests illegal usage of the LOCAL and MACRO-EXITM-ENDM
; directives.

	LIST	P=16C54

	EXITM			; Unmatched
	ENDM			; Unmatched

	LOCAL			; No operand
Label1	LOCAL	AA		; Illegal label

Mac1	MACRO			; Missing ENDM
	NOP

	END
