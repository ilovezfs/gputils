; SZ84A.ASM

; This file tests for the processor-specific items for the selected
; processor.

        LIST    P=16F84A, R=HEX

MAXROM	EQU	03FF
MAXRAM  EQU     00CF

	RETLW	12		; 0812 = 12-BIT
				; 3412 = 14-BIT
				; B612 = 16-BIT

	MOVWF	MAXRAM		; OK
	MOVWF	MAXRAM+1	; MESSAGE

	ORG	MAXROM
	NOP			; OK
	NOP			; ERROR

   IFDEF __16F84A               ; EE Processors
	ORG	20FF
	DE	22		; ERROR

	ORG	2100
	FILL	33, 40		; OK
	DE	44		; ERROR
   ENDIF

	END
