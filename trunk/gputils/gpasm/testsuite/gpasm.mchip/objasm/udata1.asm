; UDATA1.ASM

; This file tests legal usage of the UDATA directive.
; This file is placed in the object file comparison test section.

		list	p=17C44

		udata		; Unnamed, no address
Var1		res	1	; Reserve data

DATA1		udata		; Name, no address
		res	1	; Data byte

DATA2		UDATA		; Capital letters
		res	3	; Multiple bytes

DATA3		udata	0x20	; Address specified
		res	4	; Multiple words

LongDataName	udata	0x30	; Long name
		res	2	; Data word

		end