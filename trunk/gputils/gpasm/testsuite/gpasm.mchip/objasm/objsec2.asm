; OBJSEC2.ASM

; This file checks to make sure the correct section is generated for 
; the 17Cxx configuration bits - with a byte address above 0xFFFF.

	list	p=17c756

	__config	0x1234

	code	0
	data	1, 2, 3, 4, 5

	end
