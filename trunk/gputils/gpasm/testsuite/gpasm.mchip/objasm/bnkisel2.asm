; BNKISEL2.ASM

; This file tests illegal usage of the BANKISEL directive.
; It should be placed in the list comparison section for
; object files.

	list	p=17c44

	udata	
Var1	res	1
Var2	res	1


	bankisel	Var1		; Outside CODE section


	code

	bankisel			; No label specified

	bankisel	Var1+Var2	; Too complex

	bankisel	Var3		; Undefined label

	end
