; BNKSEL2.ASM

; This file tests illegal usage of the BANKSEL directive.
; It should be placed in the list comparison section for
; object files.

	list	p=17c44

	udata	
Var1	res	1
Var2	res	1


	banksel		Var1		; Outside CODE section


	code

	banksel				; No label specified

	banksel		Var1+Var2	; Too complex

	banksel		Var3		; Undefined label

	end
