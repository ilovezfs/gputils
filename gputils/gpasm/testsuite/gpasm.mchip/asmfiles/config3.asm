; CONFIG3.ASM

; This file tests the __CONFIG directive for 16-bit core.

	LIST	P=17C44, R=HEX, F=INHX32

	__CONFIG	1234

	END
