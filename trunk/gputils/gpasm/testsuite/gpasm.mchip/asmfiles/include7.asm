; INCLUDE7.ASM

; This file tests including source files.

	LIST	P=17C42

	DATA	"Include File 7"

	INCLUDE			; No file name
Label	INCLUDE "include6.asm"	; Illegal label
	INCLUDE "missing.asm"	; File not found

	MESSG	"FILE 7 - TOO MANY"
	INCLUDE	"include1.asm"

	END
