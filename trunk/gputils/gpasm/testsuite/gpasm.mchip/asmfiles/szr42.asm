; SZR42.ASM

; This file tests for the processor-specific items for the selected
; processor.

	LIST	P=17CR42, R=HEX, F=INHX32

MAXROM	EQU	0FFFF
MAXRAM	EQU	00FF

	RETLW	12		; 0812 = 12-BIT
				; 3412 = 14-BIT
				; B612 = 16-BIT

	MOVWF	MAXRAM		; OK
	MOVWF	MAXRAM+1	; MESSAGE

	ORG	MAXROM
	NOP			; OK
	NOP			; ERROR

	END
