; CONFIG6.ASM

; This file tests using the __CONFIG directive with too large a value.

	LIST	P=17C44, R=HEX, F=INHX32

	__CONFIG	12345

	END
