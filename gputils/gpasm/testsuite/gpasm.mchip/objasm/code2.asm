; CODE2.ASM

; This file tests illegal usage of the CODE directive.
; It should be placed in the list comparison section for
; object files.

	list	p=17c44

Code1	code			; Two segments, same name
	nop
Code1	code
	nop

Code2	code	xyz		; Illegal address

	end