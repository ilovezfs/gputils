; IDLOCS3.ASM

; This file tests the __IDLOCS directive for 16-bit core - illegal.

	LIST	P=17C44, R=HEX, F=INHX32

	__IDLOCS	1234

	END
