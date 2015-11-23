
	processor 16f877a

	extern	func_refd
	extern	func_selfrefd
	extern	func_nonrefd

main_VEC_reset.code	code	0
	pagesel	main
	goto	main

main.code	code

main:
	pagesel	func_refd
	call	func_refd
lock:
	pagesel	lock
	goto	lock

	END
