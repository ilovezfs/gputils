; PAGSEL2.ASM

; This file tests illegal uses of the PAGESEL directives.
; It should be placed in the list comparison section for
; object files.

	list	p=17c44

	pagesel	Addr1		; Outside code section


	code
Addr1
	pagesel				; No label specified
Addr2
	pagesel		Addr1+Addr2	; Too complex

	pagesel		Addr3		; Undefined label

	end
