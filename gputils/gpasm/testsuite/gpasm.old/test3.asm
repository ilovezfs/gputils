	;; This test should produce the following output:
	
	;; This is a test:
	;; 17 16 15 14 13 12 11 10 0F 0E 0D 0C 0B 0A 09 08 07 06 05 04 03 02 01
	;; 67 81
	;; 00 01 03 06 0A 0F 15 1C 24 2D 37 42 4E 5B 69 78 88 99 AB BE
	
	processor pic16c84
	
	include	p16c84.inc

putchar	macro
	movwf	PORTA
	endm
	
	radix	dec
	
	cblock	0xc
	index
	scratch1
	tri
	endc
	
	org	0

	;; Title message
	call	write_message

	;; Count from 23 to 1 in hex
	movlw	23
	movwf	index
loop2	movf	index,w
	call	writebyte
	call	space1
	movfw	index
	addlw	-1
	movwf	index
	bnz	loop2
	call	newline

	;; Some math that should print "67 81"
	;;				W	index
	movlw	h'12'		;	12
	movwf	index		;	12	12
	incf	index,f		;	12	13
	setc			;
	rrf	index,f		;	12	89
	xorwf	index,w		;	9B	89
	swapf	index,f		;	9B	98
	andwf	index,w		;	98	98
	clrc
	rlf	index,f		;	98	30
	iorwf	index,w		;	B8	30
	comf	index,f		;	B8	CF
	sublw	5		;	4D	CF
	subwf	index,f		;	4D	82
	xorlw	h'2D'		;	60	82
	andlw	h'F0'		;	60	82
	iorlw	h'07'		;	67	82
	nop
	decf	index,f		;	67	81

	call	writebyte
	call	space1
	movfw	index
	call	writebyte
	call	newline
	
	;; Write out the first 20 triangular numbers in a slightly sneaky way
	;; Exercises incfsz
	movlw	-20
	movwf	index
	clrf	tri
triloop
	movfw	tri
	call	writebyte
	call	space1
	
	movfw	index
	addlw	21		;  So we add 1 first time round
	addwf	tri,f		;  Should default to f
	
	incfsz	index,f
	goto	triloop

	call	newline

	sleep

	;; Write out the title message
write_message
	clrf	index
loop1	movf	index,W
	call	readit
	addlw	0
	skpnz
	return	
	putchar
	incf	index,f
	goto	loop1
readit	addwf	PCL,f
	dt	"This is a test:\n",0

newline
	movlw	d'10'
	putchar
	return
	
space1
	movlw	' '
	putchar
	return
	
	;; Hex output subroutines
writebyte
	movwf	scratch1
	swapf	scratch1,w
	call	writenibble
	movfw	scratch1
writenibble
	andlw	0xf
	addlw	-d'10'		    ;	-a	-1	0	6
	skpnc			    ;	
	addlw	('A' - '0' - d'10') ;	-a	-1	7	c
	addlw	(d'10' + '0')	    ;	30	39	41	45
	putchar
	return

	end
