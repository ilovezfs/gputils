; ALLMES10.ASM

; This file generates more messages that are created when assembly an
; object file.

	list	p=16c54

	udata
Var	res	1

	code
Label
	pagesel	Label		; Message 312 Page or Bank selection...
	banksel	Var		; Message 312 Page or Bank selection...

	end
