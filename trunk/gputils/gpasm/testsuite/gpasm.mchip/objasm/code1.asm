; CODE1.ASM

; This file tests legal usage of the CODE directive.
; This file is placed in the object file comparison test section.

		list	p=17C44

		code		; Unnamed, no address
		data	1

CODE1		code		; Name, no address
		data	2

CODE2		CODE		; Capital letters
		data	3

CODE3		code	0x20	; Address specified
		data	4

LongCodeName	code	0x30	; Long name
		data	5

		end