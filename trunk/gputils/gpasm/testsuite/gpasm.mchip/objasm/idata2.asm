; IDATA2.ASM

; This file tests illegal usage of the IDATA directive.
; It should be placed in the list comparison section for
; object files.

	list	p=17c44

Data1	idata			; Two segments, same name
	res	1
Data1	idata
	res	1

Data2	idata	xyz		; Illegal address

Data3	idata
	nop			; Program code

	end