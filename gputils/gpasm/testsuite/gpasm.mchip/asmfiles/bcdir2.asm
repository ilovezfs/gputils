; BCDIR2.ASM

; This file tests illegal usage of the BCDirect directive.

	LIST	P=16C54

	BCDIRECT
	BCDIRECT	A
	BCDIRECT	A,
	BCDIRECT	"This is string."
	BCDIRECT	,"This is string."
	
	END
