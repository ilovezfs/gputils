
	list	p=16c84
	
scale	macro	reg, factor
	if	(factor == 1)
		movwf	reg
	else
		local	low_bit
	
		scale	reg, (factor / 2)
		rlf	reg,f
		if	((factor & 1) == 1)
			addwf	reg,f
		endif
	endif
	endm

	org	0
	
	scale	h'0c',1
	scale	h'0c',h'10'
	scale	h'0c',h'55'
	scale	h'0c',h'a'

scale	macro
	endm
	
	end
	