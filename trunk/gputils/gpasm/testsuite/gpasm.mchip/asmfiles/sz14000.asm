; SZ14000.ASM

; This file tests for the processor-specific items for the selected
; processor.

	LIST	P=PIC14000, R=HEX

MAXROM	EQU	0FFF
MAXRAM	EQU	007F

	RETLW	12		; 0812 = 12-BIT
				; 3412 = 14-BIT
				; B612 = 16-BIT

	MOVWF	MAXRAM		; OK
	MOVWF	MAXRAM+1	; MESSAGE

	ORG	MAXROM
	NOP			; OK
	NOP			; ERROR

	END
