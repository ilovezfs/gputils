; BADRAM3.ASM

; This file tests illegal usage of the __MAXRAM and __BADRAM directives.

	LIST	P=16C622

	__BADRAM	0	; __MAXRAM needed first
	MOVWF		0	; Should be OK

	__MAXRAM		; Need parameter
	__MAXRAM	7E	; Too small
	__MAXRAM	1000	; Too large
	__MAXRAM	7F	; OK, set for below

	__BADRAM		; Need parameter
	__BADRAM	080	; Too large
	__BADRAM	,	; Missing argument
	__BADRAM	-	; Missing argument
	__BADRAM	1,	; Missing argument
	__BADRAM	-2	; Missing argument
	__BADRAM	3-	; Missing argument
	__BADRAM	5-4	; First less than last


	END
