; VAR1.ASM

; This file tests legal usage of the VARIABLE directive.

	LIST	P=16C54, R=HEX

#define Value1	10
Value2	=	20

	VARIABLE	Var01=1		; One with hardcoded value
	VARIABLE	Var02 = Value1	; One with string substitution
	VARIABLE	Var03 = Value2	; One with variable equate

	VARIABLE	Var04 = Var01 + Var02 + Var03
						; One with expression

	VARIABLE	Var05=5,Var06=6,Var07=7
						; Multiple squished, hardcoded

	VARIABLE	Var08 = 8 , Var09 = Value1 , Var10 = Value2 , Var11 = 1 + 2
						; Multiple mishmash

	VARIABLE	Var12			; Define later
	Var12 = 12

	DATA	Var01, Var02, Var03, Var04
	DATA	Var05, Var06, Var07, Var08
	DATA	Var09, Var10, Var11, Var12

	END
	
