; IDATA1.ASM

; This file tests legal usage of the IDATA directive.
; This file is placed in the object file comparison test section.

		list	p=17C44

		idata		; Unnamed, no address
Var1		res	1	; Reserve data

DATA1		idata		; Name, no address
		db	0x12	; Data byte

DATA2		IDATA		; Capital letters
		db	1, 2, 3	; Multiple bytes

DATA3		idata	0x20	; Address specified
		dw	0x1234, 0x5678	; Multiple words

LongDataName	idata	0x30	; Long name
		data	0x9abc	; Data word

		end