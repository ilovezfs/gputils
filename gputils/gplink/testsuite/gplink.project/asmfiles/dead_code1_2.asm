
	processor 16f877a

	global	func_refd
	global	func_selfrefd
	global	func_nonrefd

udata_shr_dead_temp	udata_shr
temp	res	1

code_live_A	code

func_refd:

	retlw	0xAB
	return

code_dead_A	code

func_selfrefd:

	movwf	temp
	pagesel	__2
	btfss	temp, 7
	goto	__2

	retlw	0x12
__2:
	retlw	0x21

code_dead_B	code

func_nonrefd:

	retlw	0xBA

	end
