; INCLUDE4.ASM

; This file tests including source files.

	LIST	P=17C42

	DATA	"Include File 4"

	MESSG	"FILE 4"
	INCLUDE	"include5.asm"

	END
