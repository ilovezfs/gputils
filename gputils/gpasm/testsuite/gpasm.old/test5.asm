proto   equ     0
        if      proto == 1
port    equ     45
sayit	macro
	messg   "one"
	endm
        else
port    equ     67
sayit	macro
	messg   "zero"
	endm
        endif

	sayit

	end	
