; UDATAS1.ASM

; This file tests legal usage of the UDATA_SHR directive.
; This file is placed in the object file comparison test section.

		list		p=17C44

		udata_shr		; Unnamed, no address
Var1		res		1	; Reserve data

DATA1		udata_shr		; Name, no address
		res		1	; Data byte

DATA2		UDATA_SHR		; Capital letters
		res		3	; Multiple bytes

DATA3		udata_shr	0x20	; Address specified
		res		4	; Multiple words

LongDataName	udata_shr	0x30	; Long name
		res		2	; Data word

		end