; OBJSEC3.ASM

; This file checks to make sure the correct sections are generated for 
; the 16Cxx configuration bits and ID locations

	list	p=16c74a

	__idlocs	0x5678
	__config	0x1234

	code	0
	data	1, 2, 3, 4, 5

	end
