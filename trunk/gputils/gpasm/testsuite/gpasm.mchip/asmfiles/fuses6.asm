; FUSES6.ASM

; This file tests using the __FUSES directive with too large a value.

	LIST	P=17C44, R=HEX, F=INHX32

	__FUSES	12345

	END
