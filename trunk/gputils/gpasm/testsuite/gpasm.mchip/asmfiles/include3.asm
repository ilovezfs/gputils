; INCLUDE3.ASM

; This file tests including source files.

	LIST	P=17C42

	DATA	"Include File 3"

	MESSG	"FILE 3"
	INCLUDE	"include4.asm"

	END
