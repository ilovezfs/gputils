; CBLOCK2.ASM

; This file tests illegal usage of CBLOCK.

	LIST	P=16C54,R=HEX

	ENDC			; Unmatched

	CBLOCK	0
		NOP		; Illegal symbol
	ENDC

	CBLOCK	0x10
		Symbol1
#define TestBit	Symbol1,2	; Embedded directive
	ENDC

	CBLOCK	0x20		; Missing ENDC
		Symbol2

	END

		
