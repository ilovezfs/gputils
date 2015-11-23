
	processor 16f877a

	global	func_refd
	global	func_selfrefd
	global	func_nonrefd

dead_code_tmp.udata_shr	udata_shr
temp	res	1

live_code_A.code	code

func_refd:

	retlw	0xAB
	return

dead_code_A.code	code

func_selfrefd:

	movwf	temp
	btfss	temp, 7
	pagesel	__2
	goto	__2

	retlw	0x12
__2:
	retlw	0x21

dead_code_B.code	code

func_nonrefd:

	retlw	0xBA

	end
