     list p=sx20,f=inhx8m

insn macro file
     ADDWF file,1 
     ANDLW 0xff
     ANDWF file,1
     BCF file,7
     BSF file,7
     BTFSC file,1
     BTFSS file,1
     CALL 0xff
     CLRF file
     CLRW
     CLRWDT
     COMF file,1
     DECF file,1
     DECFSZ file,1
     GOTO 0x100
     INCF file,1
     INCFSZ file,1
     IORLW 0xff
     IORWF file,1
     MOVF  file,1
     MOVLW 0xff
     MOVWF file
     NOP
     OPTION 
     RETLW 0
     RLF file,1
     RRF file,1
     SLEEP
     SUBWF file,1
     SWAPF file,1
     TRIS 7
     XORLW 0xff
     XORWF file,1
     ; SX only
     BANK 0xff
     IREAD
     MODE 0xf
     MOVMW
     MOVWM
     PAGE 0xff
     RETI
     RETIW
     RETP
     RETURN
     endm

     org 0
     insn 10
     org 400
     insn 20
     org 800
     insn 25
     end
