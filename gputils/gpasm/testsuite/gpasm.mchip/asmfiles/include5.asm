; INCLUDE5.ASM

; This file tests including source files.

	LIST    P=17C42

	DATA    "Include File 5"

	MESSG   "FILE 5"
	INCLUDE "include6.asm"

	END
