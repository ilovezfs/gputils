; SZ62A.ASM

; This file tests for the processor-specific items for the selected
; processor.

	LIST	P=16C62A, R=HEX

MAXROM	EQU	07FF
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
