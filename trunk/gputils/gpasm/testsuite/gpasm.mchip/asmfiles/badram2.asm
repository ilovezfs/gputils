; BADRAM2.ASM

; This file tests legal usage of the __MAXRAM and __BADRAM directives.

	LIST	P=16C622

	__maxram	7f
	__MAXRAM	0bf


	__badram	0
	__BADRAM	1  -  1
	__badram	2-3
	__badram	4,5,6
	__BADRAM	7 , 8 , 9
	__BADRAM	0a,0b-0c
Hole = 0d
	__BADRAM	Hole
	
i = 0
	while i <= 0e	; all warnings except last
	   movwf	i
i++
	endw

	END
