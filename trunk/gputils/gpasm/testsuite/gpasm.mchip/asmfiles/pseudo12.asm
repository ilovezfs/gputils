; PSEUDO12.ASM

; This file tests the special instructions for the 12-bit core.

	LIST	P=16C5X,R=HEX

	CLRC
	BCF      3,0

	SETC
	BSF      3,0

	CLRDC
	BCF      3,1

	SETDC
	BSF      3,1

	CLRZ
	BCF      3,2

	SETZ
	BSF      3,2

	SKPC
	BTFSS    3,0

	SKPNC
	BTFSC    3,0

	SKPDC
	BTFSS    3,1

	SKPNDC
	BTFSC    3,1

	SKPZ
	BTFSS    3,2

	SKPNZ
	BTFSC    3,2

	TSTF 5
	MOVF     5,1

	MOVFW 6
	MOVF     6,0

	NEGF 7,1
	COMF     7,1
	INCF     7,1

	ADDCF 8,0
	BTFSC    3,0
	INCF     8,0

	SUBCF 9,1
	BTFSC    3,0
	DECF     9,1

	ADDDCF 2,0
	BTFSC    3,1
	INCF     2,0

	SUBDCF 3,1
	BTFSC    3,1
	DECF     3,1

	B 100
	GOTO     100

	BC 110
	BTFSC    3,0
	GOTO     110

	BNC 120
	BTFSS    3,0
	GOTO     120

	BDC 130
	BTFSC    3,1
	GOTO     130

	BNDC 140
	BTFSS    3,1
	GOTO     140

	BZ 150
	BTFSC    3,2
	GOTO     150

	BNZ 160
	BTFSS    3,2
	GOTO     160

	LCALL    01
	BCF 3,5
	BCF 3,6
	CALL     01

	LCALL    234
	BSF 3,5
	BCF 3,6
	CALL     234

	LCALL    456
	BCF 3,5
	BSF 3,6
	CALL     456

	LCALL    678
	BSF 3,5
	BSF 3,6
	CALL     678

;***** Test for SSR 2061 **************************************************

	LGOTO   01
	BCF 3,5
	BCF 3,6
	GOTO    01

	LGOTO   123
	BCF 3,5
	BCF 3,6
	GOTO    123

	LGOTO   234
	BSF 3,5
	BCF 3,6
	GOTO    234

	LGOTO   345
	BSF 3,5
	BCF 3,6
	GOTO    345

	LGOTO   456
	BCF 3,5
	BSF 3,6
	GOTO    456

	LGOTO   567
	BCF 3,5
	BSF 3,6
	GOTO    567

	LGOTO   678
	BSF 3,5
	BSF 3,6
	GOTO    678

;***** End Test ***********************************************************

;     05/06/01 Craig Franklin - removed microchip internal tests
;	HALT

	RETURN

	END
