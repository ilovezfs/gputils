; UDATA2.ASM

; This file tests illegal usage of the UDATA directive.
; It should be placed in the list comparison section for
; object files.

	list	p=17c44

Data1	udata			; Two segments, same name
	res	1
Data1	udata
	res	1

Data2	udata	xyz		; Illegal address

Data3	udata
	nop			; Program code
	db	1		; Data byte

	end