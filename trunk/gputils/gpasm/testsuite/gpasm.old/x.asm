	processor 16f84
	
_halt	macro
	local	loop
loop:	goto	loop
	endm

	nop
	_halt
	nop

	end
