;*****************************************************************************************
;			GetChar Function
;  Receives a Byte Of Data
;		When reception is complete, _rcvOver Bit is cleared
;		The received data is in RxReg
;
;  Program Memory :	15 locations (17 locations if PARITY is used)
;  Cycles	  :	16 (18 if PARITY is USED)
;
;*****************************************************************************************
GetChar:
	bcf	STATUS,RP0
	bsf	_rcvOver		; Enable Reception, this bit gets reset on Byte Rcv Complete
	LOAD_BITCOUNT
        clrf	RxReg
	bcf	_FrameErr
        bcf	_ParityErr	     ; Init Parity & Framing Errors
	clrf	TMR0
	clrwdt
	bsf	STATUS,RP0
	movlw	07h
	movwf	OPTION_REG
	bcf	STATUS,RP0
	clrf	TMR0
	bsf	STATUS,RP0
	movlw	0Fh
	movwf	OPTION_REG
	clrwdt
        movlw	_OPTION_SBIT	     ; Inc On Ext Clk Falling Edge
	movwf	OPTION_REG		     ; Set Option Reg Located In Page 1
	bcf	STATUS,RP0		     ; make sure to select Page 0
	movlw	0xFF
	movwf	TMR0		     ; A Start Bit will roll over RTCC & Gen INT
	bcf	INTCON,T0IF
	bsf	INTCON,T0IE		     ; Enable RTCC Interrupt
	retfie			     ; Enable Global Interrupt
;
;*****************************************************************************************
;			Internal Subroutine
; entered from Interrupt Service Routine when Start Bit Is detected.
;
;  Program Memory :	14 locations
;  Cycles	  :	12 (worst case)
;
;*****************************************************************************************
_SBitDetected:
	bcf	STATUS,RP0
	btfsc	RX_Pin				; Make sure Start Bit Interrupt is not a Glitch
	goto	_FalseStartBit			; False Start Bit	
	bsf	_rcvProgress
	clrf	TMR0
	clrwdt
	bsf	STATUS,RP0
	movlw	07h
	movwf	OPTION_REG
	bcf	STATUS,RP0
	clrf	TMR0
	bsf	STATUS,RP0
	movlw	0Fh
	movwf	OPTION_REG
	clrwdt
	movlw	(_BIT1_INIT | SBitPrescale)	; Switch Back to INT Clock
	movwf	OPTION_REG				; Set Option Reg Located In Page 1
	bcf	STATUS,RP0				; make sure to select Page 0
	LOAD_RTCC  1,(SBitRtccLoad), SBitPrescale
	goto	RestoreIntStatus
;
_FalseStartBit:
	movlw	0xFF
        movwf	TMR0				; reload RTCC with 0xFF for start bit detection
	goto	RestoreIntStatus
;
;*****************************************************************************************
;			Internal Subroutine
; entered from Interrupt Service Routine when Start Bit Is detected.
;
;  Program Memory :	28 locations ( 43 locations with PARITY enabled)
;  Cycles	  :     24 Worst Case 
;
;*****************************************************************************************
_RcvNextBit:
	clrwdt
	bsf	STATUS,RP0
	movlw	07h
	movwf	OPTION_REG
	bcf	STATUS,RP0
	clrf	TMR0
	clrwdt
	bsf	STATUS,RP0
	movlw	07h
	movwf	OPTION_REG
	bcf	STATUS,RP0
	clrf	TMR0
	bsf	STATUS,RP0
	movlw	0Fh
	movwf	OPTION_REG
	clrwdt
	movlw	(_OPTION_INIT | RtccPrescale)	; Switch Back to INT Clock
	movwf	OPTION_REG				; Set Option Reg Located In Page 1
;
	bcf	STATUS,RP0
	movf	PORTA,W	; read RX pin immediately into WREG
	movwf	RxTemp
	LOAD_RTCC  0,RtccPreLoad, RtccPrescale	; Macro to reload RTCC
	movf	PORTA,W
	xorwf	RxTemp,W
	andlw	RX_MASK		; mask for only RX PIN (RA4)
	btfsc	STATUS,Z
	goto	_PinSampled	; both samples are same state
_SampleAgain:
	movf	PORTA,W
	movwf	RxTemp		; 2 out of 3 majority sampling done
_PinSampled:
	movf	BitCount,1
	btfsc	STATUS,Z
	goto	_RcvP_Or_S
;
	decfsz	BitCount, F
	goto	_NextRcvBit
;
_RcvP_Or_S:
   if _PARITY_ENABLE
	decfsz	ExtraBitCount
        goto	_RcvParity
   endif
;
_RcvStopBit:
	btfss	RX
	bsf	_FrameErr		; may be framing Error or Glitch        
	bcf	INTCON,T0IE			; disable further interrupts
	bcf	_rcvProgress
	bcf	_rcvOver		; Byte Received, Can RCV/TXMT an other Byte
  if _PARITY_ENABLE
	movf	RxReg,W
	call	GenParity		; Generate Parity, for Parity check
	movlw	0
	btfsc	_parityBit
	movlw	0x10			; to mask off Received Parity Bit in _ParityErr
	xorwf	SerialStatus 		; _ParityErr bit is set accordingly
  endif
  if _DataBits == 7
	rrf	RxReg,1
	bcf	RxReg,7
  endif
	goto	RestoreIntStatus
;
_NextRcvBit:
	bcf	STATUS,C
	btfsc	RX			; prepare bit for shift
	bsf	STATUS,C	
	rrf	RxReg, F		; shift in received data	
	goto	RestoreIntStatus
;
  if _PARITY_ENABLE
_RcvParity:
	bcf	_ParityErr   		; Temporarily store PARITY Bit in _ParityErr 
	btfsc	RX			; Sample again to avoid any glitches
	bsf	_ParityErr
	goto	RestoreIntStatus
  endif
;
;*****************************************************************************************
