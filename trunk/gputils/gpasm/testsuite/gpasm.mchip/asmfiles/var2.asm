; VAR2.ASM

; This file tests illegal usage of the VARIABLE directive.

	LIST	P=16C54, R=HEX

	VARIABLE			; No parameters
	VARIABLE	Const2=		; Missing value
	VARIABLE	Const3 = 1 +	; Illegal expression
	VARIABLE	Const4 = Stupid	; Undefined symbol

	END
	
