        LIST    P=18C452,R=HEX

MAXROM = 03FFF
MAXRAM = 0FFF
F       EQU     1
W       EQU     0

	ADDLW	0
	ADDLW	'A'
	ADDLW	"A"
	ADDLW	0FF

	ADDWF	0
	ADDWF	0,1
	ADDWF	0,F
	ADDWF	0,0
	ADDWF	0,W
	ADDWF	MAXRAM

	ADDWFC	0
	ADDWFC	0,1
	ADDWFC	0,F
	ADDWFC	0,0
	ADDWFC	0,W
	ADDWFC	MAXRAM

	ANDLW	0
	ANDLW	'A'
	ANDLW	"A"
	ANDLW	0FF

	ANDWF	0
	ANDWF	0,1
	ANDWF	0,F
	ANDWF	0,0
	ANDWF	0,W
        ANDWF   MAXRAM

        BC      0
        BC      0FE

     	BCF	0,0
	BCF	0,1
	BCF	0,2
	BCF	0,3
	BCF	0,4
	BCF	0,5
	BCF	0,6
	BCF	0,7
	BCF	MAXRAM,0

        BN      0
        BN      0FE

        BNC     0
        BNC     0FE

        BNN     0
        BNN     0FE     ;10

        BNOV    0
        BNOV    0FE

        BNZ     0
        BNZ     0FE

        BOV     0
        BOV     0FE

        BRA     0
        BRA     0FE

	BSF	0,0
	BSF	0,1
	BSF	0,2
	BSF	0,3
	BSF	0,4
	BSF	0,5
	BSF	0,6
	BSF	0,7
	BSF	MAXRAM,0

	BTFSC	0,0
	BTFSC	0,1
	BTFSC	0,2
	BTFSC	0,3
	BTFSC	0,4
	BTFSC	0,5
	BTFSC	0,6
	BTFSC	0,7
	BTFSC	MAXRAM,0

	BTFSS	0,0
	BTFSS	0,1
	BTFSS	0,2
	BTFSS	0,3
	BTFSS	0,4
	BTFSS	0,5
	BTFSS	0,6
	BTFSS	0,7
	BTFSS	MAXRAM,0

	BTG	0,0
	BTG	0,1
	BTG	0,2
	BTG	0,3
	BTG	0,4
	BTG	0,5
	BTG	0,6
	BTG	0,7
	BTG	MAXRAM,0

        BZ      0
        BZ      0FE

	CALL	0
        CALL    MAXROM-1

	CLRF	0
	CLRF	0,1
	CLRF	0,F
	CLRF	0,0
	CLRF	0,W
	CLRF	MAXRAM

        CLRWDT     

	COMF	0
	COMF	0,1
	COMF	0,F
	COMF	0,0
	COMF	0,W
	COMF	MAXRAM

	CPFSEQ	0
	CPFSEQ	MAXRAM

	CPFSGT	0
	CPFSGT	MAXRAM

	CPFSLT	0
	CPFSLT	MAXRAM

        DAW     

	DECF	0
	DECF	0,1
	DECF	0,F
	DECF	0,0
	DECF	0,W
	DECF	MAXRAM

	DECFSZ	0
	DECFSZ	0,1
	DECFSZ	0,F
	DECFSZ	0,0
	DECFSZ	0,W
	DECFSZ	MAXRAM

	DCFSNZ	0
	DCFSNZ	0,1
	DCFSNZ	0,F
	DCFSNZ	0,0
	DCFSNZ	0,W
        DCFSNZ  MAXRAM         ;30

	GOTO	0
        GOTO    MAXROM - 1

	INCF	0
	INCF	0,1
	INCF	0,F
	INCF	0,0
	INCF	0,W
	INCF	MAXRAM

	INCFSZ	0
	INCFSZ	0,1
	INCFSZ	0,F
	INCFSZ	0,0
	INCFSZ	0,W
	INCFSZ	MAXRAM

	INFSNZ	0
	INFSNZ	0,1
	INFSNZ	0,F
	INFSNZ	0,0
	INFSNZ	0,W
	INFSNZ	MAXRAM

	IORLW	0
	IORLW	'A'
	IORLW	"A"
	IORLW	0FF

	IORWF	0
	IORWF	0,1
	IORWF	0,F
	IORWF	0,0
	IORWF	0,W
	IORWF	MAXRAM


        LFSR    0,0
        LFSR    0,0FFF
        LFSR    1,0
        LFSR    1,0FFF
        LFSR    2,0
        LFSR    2,0FFF

        MOVF    MAXRAM, F
        MOVF    MAXRAM, W
        MOVF    0,F
        MOVF    0,W

        MOVFF   0,0
        MOVFF   MAXRAM,0
;        MOVFF   0,MAXRAM               ;;Illegal operations on this device (movff TOS[U | H | L])  
;        MOVFF   MAXRAM,MAXRAM

        MOVLB   0
        MOVLB   0F                ;40
        
	MOVLW	0
	MOVLW	'A'
	MOVLW	"A"
	MOVLW	0FF

	MOVWF	0
	MOVWF	MAXRAM

	MULLW	0
	MULLW	'A'
	MULLW	"A"
	MULLW	0FF
        
	MULWF	0
	MULWF	MAXRAM

        NEGF    0
        NEGF    MAXRAM

	NOP

        POP

        PUSH

        RCALL   0
        RCALL   07FE

        RESET           ;50         

	RETFIE

	RETLW	0
	RETLW	'A'
	RETLW	"A"
	RETLW	0FF

	RETURN

	RLCF	0
	RLCF	0,1
	RLCF	0,F
	RLCF	0,0
	RLCF	0,W
	RLCF	MAXRAM

	RLNCF	0
	RLNCF	0,1
	RLNCF	0,F
	RLNCF	0,0
	RLNCF	0,W
	RLNCF	MAXRAM

	RRCF	0
	RRCF	0,1
	RRCF	0,F
	RRCF	0,0
	RRCF	0,W
	RRCF	MAXRAM

	RRNCF	0
	RRNCF	0,1
	RRNCF	0,F
	RRNCF	0,0
	RRNCF	0,W
	RRNCF	MAXRAM

	SETF	0
	SETF	0,1
	SETF	0,F
	SETF	0,0
	SETF	0,W
	SETF	MAXRAM

	SLEEP

	SUBLW	0
	SUBLW	'A'
	SUBLW	"A"
        SUBLW   0FF        ;60

	SUBWF	0
	SUBWF	0,1
	SUBWF	0,F
	SUBWF	0,0
	SUBWF	0,W
	SUBWF	MAXRAM

        SUBFWB  0
        SUBFWB  0,1
        SUBFWB  0,F
        SUBFWB  0,0
        SUBFWB  0,W
        SUBFWB  MAXRAM

        SUBWFB  0
        SUBWFB  0,1
        SUBWFB  0,F
        SUBWFB  0,0
        SUBWFB  0,W
        SUBWFB  MAXRAM


	SWAPF	0
	SWAPF	0,1
	SWAPF	0,F
	SWAPF	0,0
	SWAPF	0,W
	SWAPF	MAXRAM

        TBLRD *  
        TBLRD *  
        TBLRD *+ 
        TBLRD *+ 
        TBLRD *- 
        TBLRD *- 
        TBLRD +* 
        TBLRD +* 

        TBLWT *  
        TBLWT *  
        TBLWT *+ 
        TBLWT *+ 
        TBLWT *- 
        TBLWT *- 
        TBLWT +* 
        TBLWT +* 
        
	TSTFSZ	0
	TSTFSZ	MAXRAM

	XORLW	0
	XORLW	'A'
	XORLW	"A"
	XORLW	0FF

	XORWF	0
	XORWF	0,1
	XORWF	0,F
	XORWF	0,0
	XORWF	0,W
	XORWF	MAXRAM

	END


