        LIST    P=18C452

MAXROM = 0x3FFF
MAXRAM = 0x0FFF

        ADDLW   0
        ADDLW   'A'
        ADDLW   "A"
        ADDLW   0xFF

        ADDWF   0
        ADDWF   0,0,A
        ADDWF   0,W,B
        ADDWF   0,1,A
        ADDWF   0,F,B
        ADDWF   0xF80
        ADDWF   MAXRAM

        ADDWFC  0
        ADDWFC  0,0,A
        ADDWFC  0,W,B
        ADDWFC  0,1,A
        ADDWFC  0,F,B
        ADDWFC  0xF80
        ADDWFC  MAXRAM

        ANDLW   0
        ANDLW   'A'
        ANDLW   "A"
        ANDLW   0xFF

        ANDWF   0
        ANDWF   0,0,A
        ANDWF   0,W,B
        ANDWF   0,1,A
        ANDWF   0,F,B
        ANDWF   0xF80
        ANDWF   MAXRAM

        BC      0
        BC      0xFE

        BCF     0,0,A
        BCF     0,1,A
        BCF     0,2,A
        BCF     0,3,A
        BCF     0,4,B
        BCF     0,5,B
        BCF     0,6,B
        BCF     0,7,B
        BCF     0xF80,0
        BCF     MAXRAM,0

        BN      0
        BN      0xFE

        BNC     0
        BNC     0xFE

        BNN     0
        BNN     0xFE

        BNOV    0
        BNOV    0xFE

        BNZ     0
        BNZ     0xFE

        BOV     0
        BOV     0xFE

        BRA     0
        BRA     0xFE

        BSF     0,0,A
        BSF     0,1,A
        BSF     0,2,A
        BSF     0,3,A
        BSF     0,4,B
        BSF     0,5,B
        BSF     0,6,B
        BSF     0,7,B
        BSF     0xF80,0
        BSF     MAXRAM,0

        BTFSC   0,0,A
        BTFSC   0,1,A
        BTFSC   0,2,A
        BTFSC   0,3,A
        BTFSC   0,4,B
        BTFSC   0,5,B
        BTFSC   0,6,B
        BTFSC   0,7,B
        BTFSC   0xF80,0
        BTFSC   MAXRAM,0

        BTFSS   0,0,A
        BTFSS   0,1,A
        BTFSS   0,2,A
        BTFSS   0,3,A
        BTFSS   0,4,B
        BTFSS   0,5,B
        BTFSS   0,6,B
        BTFSS   0,7,B
        BTFSS   0xF80,0
        BTFSS   MAXRAM,0

        BTG     0,0,A
        BTG     0,1,A
        BTG     0,2,A
        BTG     0,3,A
        BTG     0,4,B
        BTG     0,5,B
        BTG     0,6,B
        BTG     0,7,B
        BTG     0xF80,0
        BTG     MAXRAM,0

        BZ      0
        BZ      0xFE

        CALL    0
        CALL    MAXROM-1

        CLRF    0
        CLRF    0,0
        CLRF    0,A
        CLRF    0,1
        CLRF    0,B
        CLRF    0xF80
        CLRF    MAXRAM

        CLRWDT

        COMF    0
        COMF    0,0,A
        COMF    0,W,B
        COMF    0,1,A
        COMF    0,F,B
        COMF    0xF80
        COMF    MAXRAM

        CPFSEQ  0
        CPFSEQ  0xF80
        CPFSEQ  MAXRAM

        CPFSGT  0
        CPFSGT  0xF80
        CPFSGT  MAXRAM

        CPFSLT  0
        CPFSLT  0xF80
        CPFSLT  MAXRAM

        DAW

        DECF    0
        DECF    0,0,A
        DECF    0,W,B
        DECF    0,1,A
        DECF    0,F,B
        DECF    0xF80
        DECF    MAXRAM

        DECFSZ  0
        DECFSZ  0,0,A
        DECFSZ  0,W,B
        DECFSZ  0,1,A
        DECFSZ  0,F,B
        DECFSZ  0xF80
        DECFSZ  MAXRAM

        DCFSNZ  0
        DCFSNZ  0,0,A
        DCFSNZ  0,W,B
        DCFSNZ  0,1,A
        DCFSNZ  0,F,B
        DCFSNZ  0xF80
        DCFSNZ  MAXRAM

        GOTO    0
        GOTO    MAXROM - 1

        INCF    0
        INCF    0,0,A
        INCF    0,W,B
        INCF    0,1,A
        INCF    0,F,B
        INCF    0xF80
        INCF    MAXRAM

        INCFSZ  0
        INCFSZ  0,0,A
        INCFSZ  0,W,B
        INCFSZ  0,1,A
        INCFSZ  0,F,B
        INCFSZ  0xF80
        INCFSZ  MAXRAM

        INFSNZ  0
        INFSNZ  0,0,A
        INFSNZ  0,W,B
        INFSNZ  0,1,A
        INFSNZ  0,F,B
        INFSNZ  0xF80
        INFSNZ  MAXRAM

        IORLW   0
        IORLW   'A'
        IORLW   "A"
        IORLW   0FF

        IORWF   0
        IORWF   0,0,A
        IORWF   0,W,B
        IORWF   0,1,A
        IORWF   0,F,B
        IORWF   0xF80
        IORWF   MAXRAM

        LFSR    0,0
        LFSR    0,0x0FFF
        LFSR    1,0
        LFSR    1,0x0FFF
        LFSR    2,0
        LFSR    2,0x0FFF

        MOVF    0,F
        MOVF    0,W
        MOVF    0xF80,F
        MOVF    0xF80,W
        MOVF    MAXRAM, F
        MOVF    MAXRAM, W

        MOVFF   0,0
        MOVFF   0,0xF80
        MOVFF   0xF80,0
        MOVFF   MAXRAM,0
;        MOVFF   0,MAXRAM               ;;Illegal operations on this device (movff TOS[U | H | L])
;        MOVFF   MAXRAM,MAXRAM
        MOVFF   0xF80,0xF80

        MOVLB   0
        MOVLB   0xF
        MOVLB   0x1F                ; Warning[202]  : Argument out of range. Least significant bits used.

        MOVLW   0
        MOVLW   'A'
        MOVLW   "A"
        MOVLW   0FF

        MOVWF   0
        MOVWF   MAXRAM

        MULLW   0
        MULLW   'A'
        MULLW   "A"
        MULLW   0xFF

        MULWF   0
        MULWF   0xF80
        MULWF   MAXRAM

        NEGF    0
        NEGF    0xF80
        NEGF    MAXRAM

        NOP

        POP

        PUSH

        RCALL   0
        RCALL   0x7FE

        RESET

        RETFIE

        RETLW   0
        RETLW   'A'
        RETLW   "A"
        RETLW   0FF

        RETURN

        RLCF    0
        RLCF    0,0,A
        RLCF    0,W,B
        RLCF    0,1,A
        RLCF    0,F,B
        RLCF    0xF80
        RLCF    MAXRAM

        RLNCF   0
        RLNCF   0,0,A
        RLNCF   0,W,B
        RLNCF   0,1,A
        RLNCF   0,F,B
        RLNCF   0xF80
        RLNCF   MAXRAM

        RRCF    0
        RRCF    0,0,A
        RRCF    0,W,B
        RRCF    0,1,A
        RRCF    0,F,B
        RRCF    0xF80
        RRCF    MAXRAM

        RRNCF   0
        RRNCF   0,0,A
        RRNCF   0,W,B
        RRNCF   0,1,A
        RRNCF   0,F,B
        RRNCF   0xF80
        RRNCF   MAXRAM

        SETF    0
        SETF    0,0
        SETF    0,A
        SETF    0,1
        SETF    0,B
        SETF    0xF80
        SETF    MAXRAM

        SLEEP

        SUBLW   0
        SUBLW   'A'
        SUBLW   "A"
        SUBLW   0xFF

        SUBWF   0
        SUBWF   0,0,A
        SUBWF   0,W,B
        SUBWF   0,1,A
        SUBWF   0,F,B
        SUBWF   0xF80
        SUBWF   MAXRAM

        SUBFWB  0
        SUBFWB  0,0,A
        SUBFWB  0,W,B
        SUBFWB  0,1,A
        SUBFWB  0,F,B
        SUBFWB  0xF80
        SUBFWB  MAXRAM

        SUBWFB  0
        SUBWFB  0,0
        SUBWFB  0,W
        SUBWFB  0,1
        SUBWFB  0,F
        SUBWFB  0xF80
        SUBWFB  MAXRAM


        SWAPF   0
        SWAPF   0,0,A
        SWAPF   0,W,B
        SWAPF   0,1,A
        SWAPF   0,F,B
        SWAPF   0xF80
        SWAPF   MAXRAM

        TBLRD*
        TBLRD*+
        TBLRD*-
        TBLRD+*

        TBLWT*
        TBLWT*+
        TBLWT*-
        TBLWT+*

        TSTFSZ  0
        TSTFSZ  0xF80
        TSTFSZ  MAXRAM

        XORLW   0
        XORLW   'A'
        XORLW   "A"
        XORLW   0FF

        XORWF   0
        XORWF   0,0,A
        XORWF   0,W,B
        XORWF   0,1,A
        XORWF   0,F,B
        XORWF   0xF80
        XORWF   MAXRAM

        END
