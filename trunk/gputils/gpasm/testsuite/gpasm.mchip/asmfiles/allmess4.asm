;ALLMESS4.ASM
;	This file creates too many code fragments in the first page after 
;	messing with the second page.

	LIST	P=17c42, F=INHX32, R=DEC
	__CONFIG	0xFFFF


Skips	macro
i = 0	
	while i < 129
	   org	$+1
	   data	i
i++
	endw
	endm

	Skips
	Skips

	end
