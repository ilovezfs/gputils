;PAGE1.ASM
;	This file tests the behavior of PAGE and SPACE directives during
;	NOLIST.

	list	p=16c54

	data	0
	space	3
	data	0
	page
	data	1

	nolist
	data	2
	space	3
	data	2
	page
	data	3

	list
	data	4
	space	3
	data	4
	page
	data	5

	nolist
	data	6
	page
	data	7
	page
	data	8
	page
	data	9

	end

