; END1.ASM

; This file tests legal usage of the END directive.

	LIST	P=16C54, R=HEX

	DATA	1, 2, 3
	END

	DATA	4, 5, 6		; This should not be assembled.
	END			; This should not be assembled.
