; SZ876.ASM

; This file tests for the processor-specific items for the selected
; processor.

        LIST    P=16F876, R=HEX ; 14-Bit core

ROMRANGE        EQU     01FFF
RAMRANGE        EQU     01FF

	RETLW	12	; 0812 = 12-BIT
				; 3412 = 14-BIT *
				; B612 = 16-BIT

	MOVWF	RAMRANGE		; OK
	MOVWF	RAMRANGE+1	; MESSAGE

	ORG	ROMRANGE
	NOP			; OK
	NOP			; Warning

   IFDEF __16F876                ; EE Processors
	ORG	2100
        FILL    33, 0x100        ; OK
        DE      99              ; ERROR
   ENDIF

	END
