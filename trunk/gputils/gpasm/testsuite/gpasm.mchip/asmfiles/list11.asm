;LIST11.ASM

;This file is used to check the generation of an INHX16 format file
;that has data above 7FFF.

	list	p=17c44, f=inhx16

	org	0
	data	1234, 2345

	org	20
	fill	9999, 100

	org	7000
	data	7890, 8901

	org	8000
	data	3456, 4567

	org	0fe00
	data	5678, 6789

	end
