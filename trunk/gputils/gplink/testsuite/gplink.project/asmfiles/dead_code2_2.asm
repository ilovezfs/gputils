
	processor 16f877a

	global	refd_A
	global	selfrefd_A
	global	crossrefd_B
	global	crossrefd_C

udata_shr_dead_temp	udata_shr
temp	res	1

code_live_A	code
refd_A:
	retlw	0x12

code_dead_A	code
selfrefd_A:
	retlw	0x34

code_dead_B	code
crossrefd_B:
	pagesel	crossrefd_C
	call	crossrefd_C
	retlw	0x56

code_dead_C	code
crossrefd_C:
	pagesel	crossrefd_B
	call	crossrefd_B
	retlw	0x78

	end
