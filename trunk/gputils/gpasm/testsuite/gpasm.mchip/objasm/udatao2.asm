; UDATAO2.ASM

; This file tests illegal usage of the UDATA_OVR directive.
; It should be placed in the list comparison section for
; object files.

	list	p=17c44

Data1	udata_ovr	0x10		; Two segments, different address
	res		1
Data1	udata_ovr	0x20
	res		1

Data2	udata_ovr	xyz		; Illegal address

Data3	udata_ovr
	nop				; Program code
	db		1		; Data byte

	end