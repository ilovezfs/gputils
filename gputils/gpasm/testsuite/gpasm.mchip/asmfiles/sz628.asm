; SZ628.ASM

; This file tests for the processor-specific items for the selected
; processor.

        LIST    P=16F628, R=HEX ; 14-Bit core

ROMRANGE        EQU     7FF
RAMRANGE        EQU     1FF

	RETLW	12	; 0812 = 12-BIT
				; 3412 = 14-BIT *
				; B612 = 16-BIT

	MOVWF	RAMRANGE		; OK
	MOVWF	RAMRANGE+1	; MESSAGE

	ORG	ROMRANGE
	NOP			; OK
	NOP			; Warning

   IFDEF __16F628                ; EE Processors
	ORG	2100
        FILL    33, 0x80        ; OK
        DE      99              ; ERROR
   ENDIF

	END
