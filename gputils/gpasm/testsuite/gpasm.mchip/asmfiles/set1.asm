; SET1.ASM

; This file tests legal usage of the SET directive.

	LIST	P=16C54, R=HEX

#DEFINE	Def1	10

Sym01	SET	1
Sym02	SET	Def1
Sym03	SET	Sym01
Sym04	SET	Sym01+Sym02+Sym03
Sym05	SET	Sym04 - Sym03

	DATA	Sym01, Sym02, Sym03, Sym04, Sym05

Sym01	SET	2		; Redefinition allowed.

	DATA	Sym01, Sym02, Sym03, Sym04, Sym05


Sym11	.SET	1
Sym12	.SET	Def1
Sym13	.SET	Sym11
Sym14	.SET	Sym11+Sym12+Sym13
Sym15	.SET	Sym14 - Sym13

	DATA	Sym11, Sym12, Sym13, Sym14, Sym15

Sym11	.SET	2		; Redefinition allowed.

	DATA	Sym11, Sym12, Sym13, Sym14, Sym15

	END
