;  This file is used to test gpdasm with the 14 bit instruction set.

     list p=16f887

	include p16f887.inc

	__config _CONFIG1, _FOSC_HS & _WDTE_OFF & _PWRTE_ON & _MCLRE_ON & _CP_OFF & _CPD_ON & _BOREN_SBODEN & _IESO_ON & _FCMEN_OFF & _LVP_ON & _DEBUG_OFF
	__config _CONFIG2, _BOR4V_BOR40V & _WRT_1FOURTH

	__idlocs 0x1234

insn macro file 
	ADDLW	0xff
	ADDWF	file, 1 
	ANDLW	0xff
	ANDWF	file, 1
	BCF	file, 7
	BSF	file, 7
	BTFSC	file, 1
	BTFSS	file, 1
	CALL	0x100
	CLRF	file
	CLRW
	CLRWDT
	COMF	file, 1
	DECF	file, 1
	DECFSZ	file, 1
	GOTO	0x100
	INCF	file, 1
	INCFSZ	file, 1
	IORLW	0xff
	IORWF	file, 1
	MOVF	file, 1
	MOVLW	0xff
	MOVWF	file
	NOP
	OPTION 
	RETFIE
	RETLW 0
	RETURN
	RLF	file, 1
	RRF	file, 1
	SLEEP
	SUBLW	0xff
	SUBWF	file, 1
	SWAPF	file, 1
	TRIS	5
	XORLW	0xff
	XORWF	file, 1
    endm

	org 0
	insn 0

	org 0x100
	insn 0x10

	org 0x400
	insn 0x20

	org 0x800
	insn 0x25

    end
