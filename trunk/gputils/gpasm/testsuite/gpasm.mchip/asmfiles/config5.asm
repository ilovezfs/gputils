; CONFIG5.ASM

; This file tests using the __CONFIG directive twice.

	LIST	P=17C44, R=HEX, F=INHX32

	__CONFIG	1234
	__CONFIG	5678

	END
