	list	p=16f84
	include <foo.inc>

	title	"haddock is a tasty fish"

keylen	equ 10
	cblock	0x600
key:	keylen
key1:
	endc

	cblock	0xc
	turkey:	100
	roast

	;;  ??

	what_is, the_matter, my_dear; ?

tennis:	1
feeble_minded:

	endc

	cblock
	another
	endc

fooyah	macro	kung
	data	kung
	data	x1
	endm

barry	EQU	h'd0'
x2	equ	h'd0'
x1	equ	h'd0'
x3	equ	h'd0'
x4	equ	h'd0'

	org 0
here:
	movlw	0x09
	goto	there
	nop
	xorlw	(there - here)
	movlw	there
	movlw	faraway
there:
	movlw	(59 - 3 - 3)
	nop

	org	0x11
	movlw	h'44'
	movlw	h'55'
	movlw	barry

	data	1
	data	1,1,1
	data	""
	data	"."
	dt	"SKIPJACK TUNA"
	dt	"SKIPJACK TUNA!"
faraway:
	dt	$,$,$

sayit	macro	x
	if ((x & 2) == 0)
		if	((x & 1) == 0)
			messg	"zero"
		else
			messg	"one"
		endif
	else
		if	((x & 1) == 0)
			messg	"two"
		else
			messg	"three"
		endif
	endif
	endm

#define foo x

	ifdef	foo
		messg	"def"
	endif
	ifndef	foo
		messg	"not def"
	endif

	nop
	fooyah	d'947947'
	nop
	messg	"Counting from zero to three:"
	sayit	0
	sayit	1
	sayit	2
	sayit	3
	;; *****************************************************************

	movlw	3
	call	faraway
	movlw	4
	call	faraway
	return
silly	nop
	dt	1,1,2,3,5,8,13,21
	if 1
	  retlw	9
	else
	retlw	4
	endif
	retlw	7

#define ARGO 16,7
	bcf	ARGO

	dt	$,$,$

	if (0x45 == h'45') && (12h == d'18')
	else
	   messg "HEX IS BROKEN"
	endif

_halt	macro
	local	loop
loop:	goto	loop
	endm

	_halt

	;; *****************************************************************

	end
