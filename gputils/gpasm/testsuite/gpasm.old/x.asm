	processor p16c84
	
halt	macro
	local	loop
loop:	goto	loop
	endm

	nop
	halt
	nop
	
	end
	