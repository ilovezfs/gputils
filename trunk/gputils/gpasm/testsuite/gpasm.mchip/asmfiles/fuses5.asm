; FUSES5.ASM

; This file tests using the __FUSES directive twice.

	LIST	P=17C44, R=HEX, F=INHX32

	__FUSES	1234
	__FUSES 5678

	END
