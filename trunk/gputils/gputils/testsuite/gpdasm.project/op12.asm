;  This file is used to test gpdasm with the 12 bit instruction set.

	list p=12c508

	include p12c508.inc

	__config 	_OSC_XT & _WDT_OFF & _CP_ON & _MCLRE_OFF

insn macro file
	ADDWF	file, 1
	ANDLW	0xff
	ANDWF	file, 1
	BCF	file, 7
	BSF	file, 7
	BTFSC	file, 1
	BTFSS	file, 1
	CALL	0xff
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
	RETLW	0
	RLF	file, 1
	RRF	file, 1
	SLEEP
	SUBWF	file, 1
	SWAPF	file, 1
	TRIS	7
	XORLW	0xff
	XORWF	file, 1
    endm

	org 0
	insn 0

	org 0x200
	insn 0x10

	org 0x400
	insn 0x20

	org 0x800
	insn 0x25
    end
