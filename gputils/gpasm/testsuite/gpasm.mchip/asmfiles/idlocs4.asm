; IDLOCS4.ASM

; This file tests using the __IDLOCS directive twice.

	LIST	P=16C54, R=HEX

	__IDLOCS	1234
	__IDLOCS	5678

	END
