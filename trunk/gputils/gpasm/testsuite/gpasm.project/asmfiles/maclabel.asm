; This file tests the fix for Bug Report 233207.

	LIST P=16F877
	include <p16f877.inc>

ENCLL EQU 5
ENCLH EQU 7

INC_LEFT macro
	local DONE_LINC
	;; increment the 16bit encoder count for the left side
	incfsz ENCLL,f ; increment lower byte
	goto DONE_LINC ; we havn't rolled over
	incf ENCLH,f ; increment high byte because we rolled over
DONE_LINC
	endm

	ORG 0
	INC_LEFT
DONE_LINC movlw 5
        INC_LEFT
	goto DONE_LINC
        end
