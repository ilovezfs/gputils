;  This file is used to test relocatable objects with the 16 bit instruction 
;  set.  Some of the expressions are possible, but not very useful.

	list	p=17c766

	extern file, file2, address

	code
	addlw	0
	addlw	'a'
	addlw	"a"
	addlw	file

	addwf	0
	addwf	0,1
	addwf	0,f
	addwf	0,0
	addwf	0,w
	addwf	file

	addwfc	0
	addwfc	0,1
	addwfc	0,f
	addwfc	0,0
	addwfc	0,w
	addwfc	file

	andlw	0
	andlw	'a'
	andlw	"a"
	andlw	file

	andwf	0
	andwf	0,1
	andwf	0,f
	andwf	0,0
	andwf	0,w
	andwf	file

	bcf	0,0
	bcf	0,1
	bcf	0,2
	bcf	0,3
	bcf	0,4
	bcf	0,5
	bcf	0,6
	bcf	0,7
	bcf	file,0

	bsf	0,0
	bsf	0,1
	bsf	0,2
	bsf	0,3
	bsf	0,4
	bsf	0,5
	bsf	0,6
	bsf	0,7
	bsf	file,0

	btfsc	0,0
	btfsc	0,1
	btfsc	0,2
	btfsc	0,3
	btfsc	0,4
	btfsc	0,5
	btfsc	0,6
	btfsc	0,7
	btfsc	file,0

	btfss	0,0
	btfss	0,1
	btfss	0,2
	btfss	0,3
	btfss	0,4
	btfss	0,5
	btfss	0,6
	btfss	0,7
	btfss	file,0

	btg	0,0
	btg	0,1
	btg	0,2
	btg	0,3
	btg	0,4
	btg	0,5
	btg	0,6
	btg	0,7
	btg	file,0

	call	0
	call	address

	clrf	0
	clrf	0,1
	clrf	0,f
	clrf	0,0
	clrf	0,w
	clrf	file

	clrwdt

	comf	0
	comf	0,1
	comf	0,f
	comf	0,0
	comf	0,w
	comf	file

	cpfseq	0
	cpfseq	file

	cpfsgt	0
	cpfsgt	file

	cpfslt	0
	cpfslt	file

	daw	0
	daw	0,1
	daw	0,f
	daw	0,0
	daw	0,w
	daw	file

	decf	0
	decf	0,1
	decf	0,f
	decf	0,0
	decf	0,w
	decf	file

	decfsz	0
	decfsz	0,1
	decfsz	0,f
	decfsz	0,0
	decfsz	0,w
	decfsz	file

	dcfsnz	0
	dcfsnz	0,1
	dcfsnz	0,f
	dcfsnz	0,0
	dcfsnz	0,w
	dcfsnz	file

	goto	0
	goto	address

	incf	0
	incf	0,1
	incf	0,f
	incf	0,0
	incf	0,w
	incf	file

	incfsz	0
	incfsz	0,1
	incfsz	0,f
	incfsz	0,0
	incfsz	0,w
	incfsz	file

	infsnz	0
	infsnz	0,1
	infsnz	0,f
	infsnz	0,0
	infsnz	0,w
	infsnz	file

	iorlw	0
	iorlw	'a'
	iorlw	"a"
	iorlw	file

	iorwf	0
	iorwf	0,1
	iorwf	0,f
	iorwf	0,0
	iorwf	0,w
	iorwf	file

	lcall	0
	lcall	address

	movfp	0,0
	movfp	file, file2

	movlb	0
	movlb	file

	movlr	0
	movlr	file

	movlw	0
	movlw	'a'
	movlw	"a"
	movlw	file

	movpf	0,0
	movpf	file, file2

	movwf	0
	movwf	file

	mullw	0
	mullw	'a'
	mullw	"a"
	mullw	file

	mulwf	0
	mulwf	file

	negw	0
	negw	0,1
	negw	0,f
	negw	0,0
	negw	0,w
	negw	file

	nop

	retfie

	retlw	0
	retlw	'a'
	retlw	"a"
	retlw	file

	return

	rlcf	0
	rlcf	0,1
	rlcf	0,f
	rlcf	0,0
	rlcf	0,w
	rlcf	file

	rlncf	0
	rlncf	0,1
	rlncf	0,f
	rlncf	0,0
	rlncf	0,w
	rlncf	file

	rrcf	0
	rrcf	0,1
	rrcf	0,f
	rrcf	0,0
	rrcf	0,w
	rrcf	file

	rrncf	0
	rrncf	0,1
	rrncf	0,f
	rrncf	0,0
	rrncf	0,w
	rrncf	file

	setf	0
	setf	0,1
	setf	0,f
	setf	0,0
	setf	0,w
	setf	file

	sleep

	sublw	0
	sublw	'a'
	sublw	"a"
	sublw	file

	subwf	0
	subwf	0,1
	subwf	0,f
	subwf	0,0
	subwf	0,w
	subwf	file

	subwfb	0
	subwfb	0,1
	subwfb	0,f
	subwfb	0,0
	subwfb	0,w
	subwfb	file

	swapf	0
	swapf	0,1
	swapf	0,f
	swapf	0,0
	swapf	0,w
	swapf	file

	tablrd	0,0,0
	tablrd	0,1,0
	tablrd	1,0,0
	tablrd	1,1,0
	tablrd	0,0,file

	tablwt	0,0,0
	tablwt	0,1,0
	tablwt	1,0,0
	tablwt	1,1,0
	tablwt	0,0,file

	tlrd	0,0
	tlrd	1,file

	tlwt	0,0
	tlwt	1,file

	tstfsz	0
	tstfsz	file

	xorlw	0
	xorlw	'a'
	xorlw	"a"
	xorlw	file

	xorwf	0
	xorwf	0,1
	xorwf	0,f
	xorwf	0,0
	xorwf	0,w
	xorwf	file

	end


