; UDATAO1.ASM

; This file tests legal usage of the UDATA_OVR directive.
; This file is placed in the object file comparison test section.

		list		p=17C44

		udata_ovr		; Unnamed, no address
Var1		res		1	; Reserve data

DATA1		udata_ovr		; Name, no address
		res		1	; Data byte

DATA2		UDATA_OVR		; Capital letters
		res		3	; Multiple bytes

DATA3		udata_ovr	0x20	; Address specified
		res		4	; Multiple words

LongDataName	udata_ovr	0x30	; Long name
		res		2	; Data word

Mult1		udata_ovr		; Multiple, second larger
		res		1
Mult1		udata_ovr
		res		2

Mult2		udata_ovr		; Multiple, first larger
		res		2
Mult2		udata_ovr
		res		1

Mult3		udata_ovr	0x10	; Multiple with address
		res		1
Mult3		udata_ovr	0x10
		res		1

		end