; RELOC3.ASM

; This file tests illegal relocation entries.
; It should be placed in the list comparison section for
; object files.

	list	p=17c44

	udata
Var1	res	1
Var2	res	1
Var3	res	1


	code

	movwf	Var1 << 1	; Too complex

	movwf	Var4		; Symbol not defined

	end