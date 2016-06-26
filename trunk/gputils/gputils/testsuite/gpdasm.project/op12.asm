;  This file is used to test gpdasm with the 12 bit instruction set.

	list p=12c508

	include p12c508.inc

	__config 	_OSC_XT & _WDT_OFF & _CP_ON & _MCLRE_OFF

insn macro file
	ADDWF	file, f
	ANDLW	0xff
	ANDWF	file, f
	BCF	file, 7
	BSF	file, 7
	BTFSC	file, 1
	BTFSS	file, 1
	CALL	0xff
	CLRF	file
	CLRW
	CLRWDT
	COMF	file, f
	DECF	file, f
	DECFSZ	file, 1
	GOTO	0x100
	INCF	file, f
	INCFSZ	file, 1
	IORLW	0xff
	IORWF	file, f
	MOVF	file, f
	MOVLW	0xff
	MOVWF	file
	NOP
	OPTION 
	RETLW	0
	RLF	file, f
	RRF	file, f
	SLEEP
	SUBWF	file, f
	SWAPF	file, f
	TRIS	7
	XORLW	0xff
	XORWF	file, f
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
