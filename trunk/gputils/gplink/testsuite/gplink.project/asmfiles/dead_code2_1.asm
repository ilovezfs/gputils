
	processor 16f877a

	extern	refd_A
	extern	selfrefd_A
	extern	crossrefd_B
	extern	crossrefd_C

code_reset	code	0x0000
	pagesel	main
	goto	main

code_main	code
main:
	pagesel	refd_A
	call	refd_A
	pagesel	$
	goto	$

	END
