; CONSTNT1.ASM

; This file tests legal usage of the CONSTANT directive.

	LIST	P=16C54, R=HEX

#define Value1	10
Value2	=	20

	CONSTANT	Const01=1		; One with hardcoded value
	CONSTANT	Const02 = Value1	; One with string substitution
	CONSTANT	Const03 = Value2	; One with variable equate

	CONSTANT	Const04 = Const01 + Const02 + Const03
						; One with expression

	CONSTANT	Const05=5,Const06=6,Const07=7
						; Multiple squished, hardcoded

	CONSTANT	Const08 = 8 , Const09 = Value1 , Const10 = Value2 , Const11 = 1 + 2
						; Multiple mishmash

	DATA	Const01, Const02, Const03, Const04
	DATA	Const05, Const06, Const07, Const08
	DATA	Const09, Const10, Const11

	END
	
