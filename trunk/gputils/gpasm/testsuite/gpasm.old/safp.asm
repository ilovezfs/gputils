	list	p=16c84, f=inhx8m

	include	p16c84.inc
	include	utility.inc
	
	__config (_CP_OFF & _PWRTE_ON & _WDT_OFF & _RC_OSC)
	
	radix	dec

#define FP_PREC	1		; Single-byte mantissa
#define FP_SIZE	(1 + FP_PREC)	; Size in bytes of an fp quantity
#define MAX_STACK_DEPTH 4	; Number of values that the stack can hold

#define FP_EXP_BIAS	64	; Bias to use for exponent

	;; The stack grows upwards in memory.  FSR points to the next empty
	;; location on the stack.  Floating point numbers are stored in memory
	;; as:
	;;	7   6   5   4   3   2   1   0
	;;	-----------------------------
	;;	|S| exponent                |	location (N)
	;;      -----------------------------
	;;      | LSB of mantissa           |   location (N + 1)
	;;	-----------------------------
	;;	.
	;;	.
	;;	.
	;;      -----------------------------
	;;      | MSB of mantissa           |   location (N + FP_REC)
	;;	-----------------------------
	;; 
	;; The Sign field contains 0 if the number is positive, 1 if it
	;; is negative.  Note the special handling of zero below.
	;; 
	;; Exponents are biased by FP_EXP_BIAS.
	;; 
	;; There is a special representation for zero: a number with a
	;; first byte (sign + exponent) containing h'00' is treated as zero.
	;;
	;; The mantissa represents a number n such that (1.0 <= n < 2.0)
	;; There is an implied '1' bit at the front of the mantissa.
	;;
	;; FP_Dup	Duplicate top
	;; FP_Drop	Eliminate top
	;; FP_Slice	Eliminate second item on stack
	;; FP_LoadIntU8	Push w as 8-bit unsigned integer on the stack
	;; FP_Load0	Push zero on the stack
	;; FP_Swap	Exchange st0 and st1
	;; 
	;; FP_Sub	Push (st0 - st1)
	;; FP_Add	Push (st0 + st1)
	;; FP_Mul	Push (st0 * st1)
	;; FP_Div	Push (st0 / st1)
	;; FP_RSub	Push (st1 - st0)
	;; FP_RDiv	Push (st1 / st0)

	cblock	h'c'

	;; Stack can hold MAX_STACK_DEPTH values in total.
	
top:	FP_SIZE
stack:	(FP_SIZE * (MAX_STACK_DEPTH - 1))

fpt0:	FP_PREC
fptf:	1

	endc

#define	FPTF_EXPONENTS_EQUAL	7	; Used in FP_Add
#define FPTF_EQUAL		6
#define FPTF_LESS		5

#define FPTF_I			7	; Used in FP_Div
#define FPTF_CORRECTED		6
	
top_exp	equ	(top)		; Top-of-stack's exponent
top_man	equ	(top + 1)	; Top-of-stack's mantissa
	
	org	0
start
	if 0
		call	FP_Clear
		movlw	0
		call	FP_LoadIntU8
x		movlw	1
		call	FP_LoadIntU8
		call	FP_Add
		goto	x
	else
		call	FP_Add
	endif

	sleep

	;; ==================================================================
FP_Clear
	clrf	FSR
	return

	;; ==================================================================
FP_Drop
	;; Copy st1 to st0
	
i = FP_PREC
	while	(i >= 0)
		decf	FSR,f
		movf	INDF,w
		movwf	(top + i)
i = i - 1
	endw

	;; Leaves FSR pointing at start of old st1, which is now the first
	;; empty location.
	
	return

	;; ==================================================================
FP_Slice
	;; Eliminate st1
	movlw	-FP_SIZE
	addwf	FSR,f
	return
	
	;; ==================================================================
FP_LoadIntU8
	movwf	fpt0		; Preserve parameter
	tstf	fpt0		; Special case for zero
	bz	FP_Load0
	call	FP_Dup		; Make space on the stack
	movf	fpt0,w		; Copy mantissa to top-of-stack
	movwf	top_man

	;; Determine position of most-significant set bit
	movlw	FP_EXP_BIAS + 7	; Largest possible exponent is 2^7
	movwf	top_exp
	clrc			; Carry will be clear throughout loop
	goto	seek1
seek0
	decf	top_exp,f
seek1	rlf	top_man,f	; Keep shifting left until we shift a 1
	bnc	seek0

i = 1
	while (i < FP_PREC)
		clrf	(top_man + i)
i = i + 1
	endw

	;; Nothing more to do - we just shifted out a 1, which is the
	;; implied 1 bit at the start of the mantissa.
	return

	;; ==================================================================
FP_Load0
	call	FP_Dup
set0	clrf	top_exp
	return

	;; ==================================================================
FP_Dup
	movf	FSR,f
	bnz	truepush
	;; Stack was empty - set it up.
	movlw	stack
	movwf	FSR
	return
truepush
	movf	top_exp,w	; XXX - FP_PREC
	movwf	INDF
	incf	FSR,f
i = 0
	while	(i < FP_PREC)
		movfw	(top_man+i)
		movwf	INDF
		incf	FSR,f
i = i + 1
	endw

	return

FP_RSub
	movlw	h'80'
	xorwf	top_exp,f		; Negate st1
	goto	FP_Add
FP_Sub
	inc	FSR,-FP_SIZE	; FSR @ st1's exponent
	movlw	h'80'
	xorwf	INDF,f		; Negate st1
	inc	FSR,FP_SIZE	; FSR @ empty
	;; ==================================================================
FP_Add
	movf	top_exp,f	; st0 zero?  then just do FP_Drop
	bz	FP_Drop
	movf	FSR,w		; st1 zero?  then just do FP_Slice
	addlw	-(FP_PREC + 1)
	movwf	FSR		; FSR @ st1's exponent
	movf	INDF,f
	bnz	realadd
	addlw	(FP_PREC + 1)
	movwf	FSR
	goto	FP_Slice
realadd
	;; We have both arguments nonzero.
	call	sort		; Make st0 the larger of the two
	inc	FSR,FP_SIZE
	call	adjust_st1	; Adjust st1 to have same exponent as st0
	inc	FSR,-FP_SIZE	; FSR @ st1's exponent
	;; The result has the same sign as st0, because st0 is larger.
	;; What we do next depends on the signs of st0 and st1. If they're
	;; the same, we call 'increase', if they differ, we call 'decrease'.
	movf	top_exp,w
	xorwf	INDF,w
	andlw	h'80'
	bnz	decrease
increase
	incf	FSR,f		; FSR @ st1's lsb
	movf	INDF,w
	addwf	top_man,f
i = 1
	while (i < FP_PREC)
		incf	FSR,f
		adc	INDF,(top_man+i)
i = i + 1
	endw
	incf	FSR,f		; FSR @ empty
	btfsc	fptf,FPTF_EXPONENTS_EQUAL
	goto	bump1
	bnc	FP_Slice	; st0 is result, get rid of st1.
	clrc
bump1	rrf	top_man,f	; Shift mantissa right 1
	incf	top_exp,f	; Make exponent 1 bigger
	goto	FP_Slice	; st0 is result, get rid of st1.
decrease
	incf	FSR,f		; FSR @ st1's lsb
	movf	INDF,w
	subwf	top_man,f
i = 1
	while (i < FP_PREC)
		incf	FSR,f
		sbc	INDF,(top_man+i)
i = i + 1
	endw
	inc	FSR,-FP_PREC	; FSR @ st1's exponent - will be empty
	;; Carry out or FPTF_EXPONENTS_EQUAL (they're mutually exclusive
	;; because the operands have been sorted above), mean that we've lost
	;; the implied '1' bit.  In this case, shift the mantissa left until
	;; there's a carry out, decrementing the exponent as we're doing it.
	bnc	find1
	btfss	fptf,FPTF_EXPONENTS_EQUAL
	return
find1
	;; See if the mantissa is zero.  If so, the answer is zero.
	;; Do this with a sequence such as for FP_PREC == 3:
	;;	tstf	(top_man+2)
	;;	skpnz
	;;	tstf	(top_man+1)
	;;	skpnz
	;;	tstf	(top_man+0)
	
i = FP_PREC - 1
	while (i >= 0)
		tstf	(top_man+i)
		if (i > 0)
			skpnz
		endif
i = i - 1
	endw
	bz	set0
find2
	clrc
i = 0
	while	(i < FP_PREC)
		rlf	(top_man+i),f	; Shift mantissa left 1
i = i + 1
	endw
	decf	top_exp,f	; Decrement exponent
	bnc	find2
	
	return
	
	;; Compare the two values at the top-of-stack.  If (|st(0)| < |st(1)|)
	;; exchange them.
	;; Input:	FSR	points to st1's exponent
	;; Output:	FSR	points to st1's exponent
sort
	clrf	fptf
	call	compare_magnitudes
	btfss	fptf,FPTF_LESS
	return
	;; We're going to swap.  Move FSR to a realistic place...
	inc	FSR,FP_SIZE	; FSR @ empty space
	;; ...and negate fpt0 - we use it to shift the smaller operand.
	comf	fpt0,f
	incf	fpt0,f
	call	FP_Swap
	inc	FSR,-FP_SIZE	; FSR @ st1's exponent
	return
	
	;; ==================================================================
FP_Mul
	;; If either argument is zero, the result is zero.
	inc	FSR,-FP_SIZE	; FSR @ st1's exp
	tstf	top_exp
	skpnz
	return
	movf	INDF,w
	bnz	true_mul
ret0	clrf	top_exp
	return
true_mul
	;; Product's exponent is sum of st0's and st1's, minus bias.
	;; Excitingly, the product's sign is also just the sum of the
	;; signs of the exponents, given that the sign bit is in position
	;; 7.
	addwf	top_exp,f
	movlw	FP_EXP_BIAS
	subwf	top_exp,f
	;; XXX - should check for under/overflow here.

	;; top_man	accumulator for result
	;; st1's man	added to top_man for every 1 bit in...
	;; fpt0		multiplier
	
	incf	FSR,f
	movf	top_man,w	; fpt0 = top_man
	movwf	fpt0
	movf	INDF,w		; top_man = st1's man

	clrf	fptf		; fptf = 1
	incf	fptf,f
	
	setc
	goto	mloop
mhead
	clrc
	rlf	fpt0,f	
	skpnc
	call	add_st1
	clrc
mloop	rrf	INDF,f
	tstf	fpt0
	bnz	mhead

	decf	FSR,f		; FSR @ st1's exponent
	
	movwf	top_man

	;; fptf can be 1, 2, or 3 now.  If bit 1 is set, we need to
	;; adjust the result.
	btfss	fptf,1
	return

	rrf	fptf,w
	rrf	top_man,f
	incf	top_exp,f

	return

add_st1
	addwf	INDF,w
	skpnc
	incf	fptf,f
	return

	;; ==================================================================
FP_RDiv
	call	FP_Swap
FP_Div
	;; If top is zero, the result is zero.
	inc	FSR,-FP_SIZE	; FSR @ st1's exp
	tstf	top_exp
	skpnz
	return
	movf	INDF,w

	;; Similar approach to FP_Mul above.
	
	subwf	top_exp,f
	movlw	FP_EXP_BIAS
	addwf	top_exp,f
	;; XXX - should check for under/overflow here.

	;; About to calculate Q = (numerator / denom).
	;; Most operations involve numerator and denom, so allocate fixed
	;; locations to these.  Use st1 (accessed via FSR) for Q, since
	;; the algortithm just shifts bits into successive bytes of Q's
	;; mantissa.  When we're done, just do FP_Drop.
	;;
	;; This allocation increases the per-divide cost, but makes
	;; the per-bit cost much less:
	;; 
	;; numerator	top
	;; denom	fpt0
	;; Q		st1

	movlw	(1 << FPTF_I)
	movwf	fptf

	;; Copy st1 to fpt0
i = 0
	while	(i < FP_PREC)
		incf	FSR,f
		movfw	INDF
		movwf	(fpt0 + i)
i = i + 1
	endw
	;; FSR @ st1's MSB
	;; From hereon, st1 will hold Q, which gets built bitwise from MSB to
	;; LSB.

	call	set_c_if_denom_le_numerator
	bnc	less_than_1

	call	subtract_denom_from_numerator
	setc
	call	rr_denom
	bcf	fptf,FPTF_I
	goto	common
	
less_than_1
	bsf	fptf,FPTF_CORRECTED
	decf	top_exp,f
	setc
	call	rr_denom
	call	subtract_denom_from_numerator
	skpc			; Clear I if we had carry out
	bcf	fptf,FPTF_I
	clrc
	call	rr_denom
common

	clrf	INDF		; This saves a cycle in the loop below
	
divloop
	setc
	btfss	fptf,FPTF_I
	call	set_c_if_denom_le_numerator
	bnc	miss
	;; Do subtract
	call	subtract_denom_from_numerator
	skpc
	bcf	fptf,FPTF_I
	setc
miss
	;; C=1 if we're to set the LSB of Q, so all we need to do is...
	rlf	INDF,f		; Sets C=0 because we cleared INDF above
	call	rr_denom

	incf	fptf,f
	btfss	fptf,3		; Count to 8
	goto	divloop

	;; If we corrected in the first place, lose the last bit of Q
	btfsc	fptf,FPTF_CORRECTED
	bcf	INDF,0
	
	;; Copy top_exp to st1, restore FSR to empty, and route
	;; through to FP_Drop
	decf	FSR,f		; FSR @ st1's exp
	movfw	top_exp
	movwf	INDF
	inc	FSR,FP_SIZE
	goto	FP_Drop

	;; Rotates denom right through C
rr_denom
i = (FP_PREC - 1)
	while	(i >= 0)
		rrf	(fpt0 + i),f
i = i - 1
	endw
	return
	
	;; returns with C=0 if carry out
subtract_denom_from_numerator
	movfw	fpt0
	subwf	top_man,f
i = 1
	while	(i < FP_PREC)
		sbc	(fpt0 + i), (top_man + i)
i = i + 1
	endw
	return

set_c_if_denom_le_numerator
i = (FP_PREC - 1)
	while	(i >= 0)
		movfw	(fpt0 + i)
		subwf	(top_man + i),w
		if	(i > 0)
			skpnz
			return
		else
			return
		endif
i = i - 1
	endw

	;; ==================================================================
FP_Swap
i = FP_PREC
	while	(i >= 0)
		decf	FSR,f
		movf	INDF,w		; Swap INDF and (top + i)
		xorwf	(top + i),w
		xorwf	(top + i),f
		xorwf	(top + i),w
		movwf	INDF
i = i - 1
	endw
	inc	FSR,FP_SIZE	; FSR @ empty space
	return

	;; ==================================================================
	;; compare_magnitudes
	;; Input:	FSR	points to st1's exponent
	;; Output:	FSR	points to st1's exponent
	;; Compares magnitude of st0 and st1, sets flags in fptf accordingly.
	;; FPTF_EXPONENTS_EQUAL if exponents are equal
	;; FPTF_EQUAL if values are equal
	;; FPTF_LESS if st0 < st1
	;; 
compare_magnitudes
	;; First, compare exponents.  If they're equal, set 
	;; FPTF_EXPONENTS_EQUAL, then go on to check the mantissae.
	movf	top_exp,w
	andlw	h'7f'
	movwf	fpt0
	movf	INDF,w
	andlw	h'7f'
	subwf	fpt0,f		; C if (fpt0 >= w), (st0 >= st1)
	bnz	inequal		; exponents differed - almost done
	bsf	fptf,FPTF_EXPONENTS_EQUAL
	;; Check the mantissae for equality
	incf	FSR,f		; FSR @ st1's lsb
	movf	top_man,w	; XXX - FP_PREC
	subwf	INDF,w
	if (FP_PREC == 1)
		bnz	compare_mantissae0
	else
		if (FP_PREC == 2)
			bnz	compare_mantissae1
		else
			bnz	compare_mantissae2
		endif
	endif

	if	(FP_PREC >= 2)
		incf	FSR,f		; 
		movf	top_man,w	; XXX - FP_PREC
		subwf	INDF,w
		if (FP_PREC == 2)
			bnz	compare_mantissae0
		else
			bnz	compare_mantissae1
		endif
	endif
	
	if	(FP_PREC == 3)
		incf	FSR,f		; FSR @ st1's msb
		movf	top_man,w	; XXX - FP_PREC
		subwf	INDF,w
		bnz	compare_mantissae0
	endif
	
	;; Mantissae were equal as well.  Set FPTF_EQUAL and return.
	inc	FSR,-FP_PREC		; FSR @ st1's exponent
	bsf	fptf,FPTF_EQUAL
	return
	if (FP_PREC == 3)
compare_mantissae2
		incf	FSR,f
	endif
	if (FP_PREC >= 2)
compare_mantissae1
		incf	FSR,f
	endif
compare_mantissae0
	;; Compare mantissae, starting at the MSB
	;; FSR @ st1's MSB
	if (FP_PREC == 1)
		movf	INDF,w
		subwf	top_man,w
		decf	FSR,f		; FSR @ st1's exponent
	else
		if (FP_PREC == 2)
			movf	INDF,w
			subwf	top_man+1,w
			bnz	inequal2
			decf	FSR,f		; FSR @ st1's LSB
			movf	INDF,w
			subwf	top_man,w
			goto	inequal1
inequal2		decf	FSR,f		; FSR @ st1's LSB
inequal1		decf	FSR,f		; FSR @ st1's exponent
		else		; FP_PREC == 3
			movf	INDF,w
			subwf	top_man+2,w
			bnz	inequal3
			decf	FSR,f		; FSR @ st1's middle byte
			movf	INDF,w
			subwf	top_man+1,w
			bnz	inequal2
			decf	FSR,f		; FSR @ st1's LSB
			movf	INDF,w
			subwf	top_man,w
			goto	inequal1
	
inequal3		decf	FSR,f		; FSR @ st1's middle byte
inequal2		decf	FSR,f		; FSR @ st1's LSB
inequal1		decf	FSR,f		; FSR @ st1's exponent
		endif
	endif
inequal
	skpc
	bsf	fptf,FPTF_LESS
	return
	
	;; ==================================================================
	;; adjust_st1
	;; 
	;; Input:	FSR @ first empty space
	;; Output:	FSR @ first empty space
	;;
	;; Adjusts st1 by shifting it right by fpt0 bits.
	;; 
adjust_st1
	movf	fpt0,f
	skpnz
	return
	decf	FSR,f		; FSR @ st1's MSB
	setc			; Shift the implied '1' in first
adjuster
i = FP_PREC - 1
	while (i >= 0)
		rrf	INDF,f		; shift right, losing low bit
		if (i > 0)
			decf	FSR,f
		endif
i = i - 1
	endw			; FSR @ st1's LSB
	
	inc	FSR,FP_PREC - 1	; FSR @ st1's MSB
	
	clrc			; shift zero in next time
	decfsz	fpt0,f
	goto	adjuster

	incf	FSR,f		; FSR @ empty

	return
	
	end
