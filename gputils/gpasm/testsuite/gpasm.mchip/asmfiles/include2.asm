; INCLUDE2.ASM

; This file tests including source files.

	LIST	P=17C42

	DATA	"Include File 2"

	MESSG	"FILE 2"
	INCLUDE	"include3.asm"

	END
