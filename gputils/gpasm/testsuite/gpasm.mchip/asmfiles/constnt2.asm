; CONSTNT2.ASM

; This file tests illegal usage of the CONSTANT directive.

	LIST	P=16C54, R=HEX

	CONSTANT			; No parameters
	CONSTANT	Const1		; No value
	CONSTANT	Const2=		; Missing value
	CONSTANT	Const3 = 1 +	; Illegal expression
	CONSTANT	Const4 = Stupid	; Undefined symbol

	END
	
