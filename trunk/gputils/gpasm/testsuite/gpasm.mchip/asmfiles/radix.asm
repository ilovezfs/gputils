; RADIX.ASM

; This file tests radix specification.  Note that command line selection
; will override source file specifications.

	LIST	P=16C54

	DATA	10

	RADIX	OCTAL
	DATA	10
	RADIX	DECIMAL
	DATA	10
	RADIX	HEX
	DATA	10

	END
