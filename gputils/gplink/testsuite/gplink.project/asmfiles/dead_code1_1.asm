
	processor 16f877a

	extern	func_refd
	extern	func_selfrefd
	extern	func_nonrefd

code_reset	code	0x0000
	pagesel	main
	goto	main

code_main	code

main:
	pagesel	func_refd
	call	func_refd
lock:
	pagesel	lock
	goto	lock

	END
