; LIST6.ASM

; This file tests legal usage of the LIST directive.

	LIST	P=17C42
	LIST	F = INHX8M	; Check generated hex file for format

	DATA	1, 2, 3, "abc"

	END

