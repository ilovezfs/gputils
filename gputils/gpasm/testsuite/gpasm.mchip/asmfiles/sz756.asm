; SZ671.ASM

; This file tests for the processor-specific items for the selected
; processor.

	LIST	P=17C756, R=HEX, F=INHX32    ; 16-Bit core

ROMRANGE	EQU	0XFFFF
RAMRANGE	EQU	00FF
EERANGE     EQU 0040

	RETLW	12	; 0812 = 12-BIT
				; 3412 = 14-BIT
				; B612 = 16-BIT *

	MOVWF	RAMRANGE	; OK
	MOVWF	RAMRANGE+1	; MESSAGE

	ORG	ROMRANGE
	NOP			; OK
	NOP			; Warning

   IFDEF    __16C822	; EE Processors
	ORG	    2100
	FILL	33, EERANGE	; OK
	DE	    44		    ; ERROR
   ENDIF

	END
