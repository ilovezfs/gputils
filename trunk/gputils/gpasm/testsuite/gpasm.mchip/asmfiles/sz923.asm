; SZ923.ASM

; This file tests for the processor-specific items for the selected
; processor.

	LIST	P=16C923, R=HEX

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

    					; EE Processors
;	ORG	20FF
;	DE	22		; ERROR

;	ORG	2100
;	FILL	33, 40		; OK
;	DE	44		; ERROR

	END
