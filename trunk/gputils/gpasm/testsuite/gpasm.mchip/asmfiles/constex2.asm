; CONSTEX2.ASM

; This file tests for errors in constant expressions.

	list	p=16c54

;***** Test for SSR 2060 **************************************************

	data	(90-'()') * 2		; Two characters in quotes
	data	(90-"()") * 2		; Two characters in quotes

;***** End Test ***********************************************************

;***** Test for SSR 1861

Var1	equ	b'10"	; stuff

;***** End Test

;***** Test for SSR 3245

w	equ	0
	goto	$-9w

;***** End Test

	end
