; PSEUDO14.ASM

; This file tests the special instructions for the 14-bit core.

;     05/06/01 Craig Franklin - removed M=0x1FFF
	LIST	P=16CXX,R=HEX

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

	Dest = 0
	LCALL Dest
	BCF	0xa, 3
	BCF	0xa, 4
	CALL     Dest

	Dest = 0800
	LCALL Dest
	BSF	0xa, 3
	BCF	0xa, 4
	CALL     Dest

	Dest = 1000
	LCALL Dest
	BCF	0xa, 3
	BSF	0xa, 4
	CALL     Dest

	Dest = 1800
	LCALL Dest
	BSF	0xa, 3
	BSF	0xa, 4
	CALL     Dest

;     05/06/01 Craig Franklin - removed microchip internal tests
;	HALT

;	SETLAT	0
;	SETLAT	1
;	SETLAT	2
;	SETLAT	3

	END
