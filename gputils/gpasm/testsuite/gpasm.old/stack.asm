	list	p=16c84

	include	p16c84.inc

	cblock	0xc

	tos:	2
	stack:	6
	t0:	1
	
	endc

	org	0
	goto	Main

Main
	call	S_Empty
	movlw	h'94'
	call	S_PushImmediate
	movlw	h'70'
	call	S_PushImmediate
	call	S_Add

;; Empty the stack
S_Empty
	clrf	FSR
	return

S_PushImmediate
	movf	FSR,f
	skpz
	goto	truepush

	;; First item on the stack.  Put it there and set up stack pointer
	movwf	tos + 1
	clrf	tos
	movlw	stack
	movwf	FSR
	return

truepush
	;; Swap W and tos+1
	xorwf	tos+1,w
	xorwf	tos+1,f
	xorwf	tos+1,w
	
	call	push1_w
	
	clrf	tos

	return

push1	movf	tos+1,w
push1_w
	incf	FSR
	movwf	INDF
	decf	FSR
	movf	tos,w
	movwf	INDF
	incf	FSR
	incf	FSR
	return

S_Add
	decf	FSR
	movf	INDF,w
	addwf	tos+1,f
	decf	FSR
	skpnc
	incf	tos
	movf	INDF,w
	addwf	tos,f
	return

S_Read
	;; Swap INDF and tos+1
	;; Copy FSR to tos via w
	;; Read FSR+1 -> w
	;; tos+1 -> INDF   w -> tos+1 
	
	movf	tos+1,w
	movwf	INDF
	end
	