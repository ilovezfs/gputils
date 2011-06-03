; ACCESSO1.ASM

; This file tests legal usage of the ACCESS_OVR directive.
; This file is placed in the object file comparison test section.

		list		p=18F242

		access_ovr		; Unnamed, no address
Var1		res		1	; Reserve data

DATA1		access_ovr		; Name, no address
		res		1	; Data byte

DATA2		ACCESS_OVR		; Capital letters
		res		3	; Multiple bytes

DATA3		access_ovr	0x20	; Address specified
		res		4	; Multiple words

LongDataName	access_ovr	0x30	; Long name
		res		2	; Data word

Mult1		access_ovr		; Multiple, second larger
		res		1
Mult1		access_ovr
		res		2

Mult2		access_ovr		; Multiple, first larger
		res		2
Mult2		access_ovr
		res		1

Mult3		access_ovr	0x10	; Multiple with address
		res		1
Mult3		access_ovr	0x10
		res		1

		end