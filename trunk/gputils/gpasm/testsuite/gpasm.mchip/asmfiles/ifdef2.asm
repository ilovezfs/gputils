; IFDEF2.ASM

; This file tests illegal usage of the IFDEF and IFNDEF directives.

	LIST	P=17C42

#define	Def1

	IFDEF			; Missing operand
	   NOP
	ENDIF

Label	IFDEF	Def2		; Illegal label
	   NOP
	ENDIF

	IFNDEF			; Missing operand
	   NOP
	ENDIF

Label	IFNDEF	Def1		; Illegal label
	   NOP
	ENDIF

	END
