; DEF2.ASM

; This file tests illegal usage of DEFINE.

	LIST	P=16C54, R=HEX

#DEFINE	Def1(A				; Missing paren and def
#DEFINE	Def2(A)				; Missing definition
#DEFINE	Def3(A)		(A+1		; Missing paren
DEFINE	Def1	10

#define Def4 (1+2)
	DATA	Def4

	DATA	Def2
	DATA	Def2(1)
	DATA	Def3(2)

	END
