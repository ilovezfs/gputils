; MACRO3.ASM

; This file tests how local variables are handled within macros.

	LIST	P=16C54

	VARIABLE	X = 22

LocMac	MACRO	Num
	LOCAL	X
	X = Num
	WHILE X > 0
	   DATA	X
	   X--
	ENDW
	ENDM

	DATA	X
	LocMac	2
	DATA	X
	LocMac	4
	DATA	X
	LocMac	3
	DATA	X

	END
