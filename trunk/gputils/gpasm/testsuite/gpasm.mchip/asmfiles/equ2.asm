; EQU2.ASM

; This file tests illegal usage of the EQU directive.

	LIST	P=16C54, R=HEX

	EQU		; No label or value
	EQU	1	; No label
Sym01	EQU		; No value
Sym02	EQU	1+	; Illegal expression
Sym03	EQU	5
Sym03	EQU	6	; Redefinition

	END
