; RELOC1.ASM

; This file tests several relocation types.  Both the object file and
; the list file should be examined.

	list	p=17c44

mac	macro	
	local	here
here	nop
	goto	here
	endm

	udata

Var	res	1
Var2	res	1


	code

Top
	movlw	Var2-Var		; should be no entry

Back	
	goto	Back
	goto	Forward
Forward

	mac

	call	Top
	call	Top+1
	call	Top-1

	goto	Top
	goto	Top+1
	goto	Top-1

	movlw	HIGH(Top)
	movlw	HIGH(Top+1)
	movlw	HIGH(Top-1)

	movlw	LOW(Top)
	movlw	LOW(Top+1)
	movlw	LOW(Top-1)
	movlw	Top

	movfp	0x20, Var
	movfp	0x20, Var+1
	movfp	0x20, Var-1

	movpf	Var, 0x20
	movpf	Var+1, 0x20
	movpf	Var-1, 0x20

	banksel	Var
	banksel	Var+1
	banksel	Var-1

	pagesel	Top
	pagesel	Top+1
	pagesel	Top-1

	data	Top
	data	Top+1
	data	Top-1

	bankisel	Var
	bankisel	Var+1
	bankisel	Var-1

	movfp	Var, 5	
	movfp	Var+1, 5	
	movfp	Var-1, 5	

	movpf	5, Var
	movpf	5, Var+1
	movpf	5, Var-1

	movwf	Var
	movwf	Var+1
	movwf	Var-1

	movlr	Var
	movlr	Var+1
	movlr	Var-1

	movlb	Var
	movlb	Var+1
	movlb	Var-1

	movfp	Var, Var2
	movfp	Var+1, Var2+2	
	movfp	Var-1, Var2-2	

	end
