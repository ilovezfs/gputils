	list	p=16c84	

	include	p16c84.inc
	__config h'3ffb'
	
	radix	dec

	;; *************************************************************

	;; *************************************************************

	org	0
start
	movlw	0
	option
	tris	PORTA
	tris	PORTB

	clrf	PORTA
	clrf	PORTB
	
loop
	comf	PORTA,f
	comf	PORTB,f

	movlw	50
ss	sleep
	addlw	-1
	skpz
	goto	ss

	goto	loop
		
	end

