; SZEE8.ASM

; This file tests for the processor-specific items for the selected
; processor.

	LIST	P=EEPROM8, R=HEX, F=INHX8M

MAXROM	EQU	007F
;MAXRAM	EQU	001F

;	RETLW	12		; 0812 = 12-BIT
;				; 3412 = 14-BIT
;				; B612 = 16-BIT

;	MOVWF	MAXRAM		; OK
;	MOVWF	MAXRAM+1	; MESSAGE

	ORG	MAXROM
;	NOP			; OK
;	NOP			; WARNING

	DATA	12		; OK
	DATA	34		; WARNING
	END
