; MACRO1.ASM

; This file tests how global labels can be generated from macros.

	LIST	P=16C54

MakLab	MACRO	Num
Label#v(Num)
	DATA	Num
	ENDM

	I = 4
	WHILE I > 0
	   MakLab	I
	   I--
	ENDW

	I = 4
	WHILE I > 0
	   GOTO	Label#V(I)
	   I--
	ENDW

	GOTO	Label1
	GOTO	Label2
	GOTO	Label3
	GOTO	Label4

	END

