; BCDIR1.ASM

; This file tests legal usage of the BCDirect directive.

	LIST	P=16C54

	BCDIRECT	A, "This is my first string."
	BCDIRECT	1, This is my second string.
	BCDIRECT	b, ""

	END
