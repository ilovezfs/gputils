; SET2.ASM

; This file tests illegal usage of the SET directive.

	LIST	P=16C54, R=HEX

	SET		; No label or value
	SET	1	; No label
Sym01	SET		; No value
Sym02	SET	1+	; Illegal expression

	END
