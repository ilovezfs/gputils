; EQU1.ASM

; This file tests legal usage of the EQU directive.

	LIST	P=16C54, R=HEX

#DEFINE	Def1	10

Sym01	EQU	1
Sym02	EQU	Def1
Sym03	EQU	Sym01
Sym04	EQU	Sym01+Sym02+Sym03
Sym05	EQU	Sym04 - Sym03

	DATA	Sym01, Sym02, Sym03, Sym04, Sym05

	END
