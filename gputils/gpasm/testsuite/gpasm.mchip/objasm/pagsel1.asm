; PAGSEL1.ASM

; This file tests legal uses of the PAGESEL directives.
; This file goes in the object file comparison test section.

	list	p=17c44

	extern	Addr1


	code

Addr2
	pagesel	Addr2		; Normal

	pagesel	Addr2 + 1	; Plus offset
Addr3
	PAGESEL	Addr3		; Upper case

	pagesel	Addr3 - 1	; Minus offset

Addr4	
	global	Addr4

	pagesel	Addr4		; Global variable

	pagesel	Addr1		; External variable

	end