; IDLOCS5.ASM

; This file tests using the __IDLOCS directive with too large a value.

	LIST	P=16C54, R=HEX

	__IDLOCS	12345

	END
