; FILL4.ASM

; This file tests illegal usage of the FILL directive.

	LIST	P=16C54, R=HEX

	FILL	0x1234, 5		; Too large for core size.

	END
