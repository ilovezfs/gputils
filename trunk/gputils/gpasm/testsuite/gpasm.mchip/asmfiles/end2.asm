; END2.ASM

; This file tests legal usage of the END directive.

	LIST	P=16C54, R=HEX

	I = 1

	DATA	1, 2, 3

	IF I == 1
  	   END
	ELSE
	   DATA	4, 5, 6		; This should not be assembled.
	   END			; This should not be assembled.
	ENDIF
