; INCLUDE1.ASM

; This file tests including source files.

	LIST	P=17C42

	DATA	"Include File 1"

	MESSG	"FILE 1"
	INCLUDE	"include2.asm"

	#INCLUDE "include5.asm"

	END
