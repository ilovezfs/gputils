; BNKISEL1.ASM

; This file tests legal uses of the BANKISEL directives.
; This file goes in the object file comparison test section.

	list	p=17c44

UDATA1	UDATA

Var1	res	1
Var2	res	1

UDATA2	UDATA

Var3	res	1
Var4	res	1
Var5	res	1

	global	Var5
	extern	Var6


	code

	bankisel	Var1		; Normal

	bankisel	Var2 + 1	; Plus offset

	BANKISEL	Var3		; Upper case

	bankisel	Var3 - 1	; Minus offset

	bankisel	Var5		; Global variable

	bankisel	Var6		; External variable

	end