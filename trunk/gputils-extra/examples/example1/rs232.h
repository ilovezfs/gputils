
		NOLIST

;*****************************************************************************************
;				RS-232 Header File
;   PIC16C6X/7X/8X
;*****************************************************************************************

_ClkOut		equ	(_ClkIn >> 2)		; Instruction Cycle Freq = CLKIN/4 
;

_CyclesPerBit	set	(_ClkOut/_BaudRate)
_tempCompute	set	(_CyclesPerBit >> 8)
;
;*****************************************************************************************
;		Auto Generation Of Prescaler & Rtcc Values
;   Computed during Assembly Time
;*****************************************************************************************

;  At first set Default values for RtccPrescale & RtccPreLoad
;
RtccPrescale	set	0
RtccPreLoad	set	_CyclesPerBit
UsePrescale	set	FALSE

 if (_tempCompute >= 1)
RtccPrescale	set	0
RtccPreLoad	set	(_CyclesPerBit >> 1)

UsePrescale	set	TRUE
 endif

 if (_tempCompute >= 2)
RtccPrescale	set	1
RtccPreLoad	set	(_CyclesPerBit >> 2)
 endif

 if (_tempCompute >= 4)
RtccPrescale	set	2
RtccPreLoad	set	(_CyclesPerBit >> 3)
 endif

 if (_tempCompute >= 8)
RtccPrescale	set	3
RtccPreLoad	set	(_CyclesPerBit >> 4)
 endif


 if (_tempCompute >= 16)
RtccPrescale	set	4
RtccPreLoad	set	(_CyclesPerBit >> 5)
 endif


 if (_tempCompute >= 32)
RtccPrescale	set	5
RtccPreLoad	set	(_CyclesPerBit >> 6)
 endif

 
 if (_tempCompute >= 64)
RtccPrescale	set	6
RtccPreLoad	set	(_CyclesPerBit >> 7)
 endif


 if   (_tempCompute >= 128)
RtccPrescale	set	7
RtccPreLoad	set	(_CyclesPerBit >> 8)
 endif

;
     if( (RtccPrescale == 0) && (RtccPreLoad < 60))
		messg	"Warning : Baud Rate May Be Too High For This Input Clock"
     endif
;
; Compute RTCC & Presclaer Values For 1.5 Times the Baud Rate for Start Bit Detection
;

_SBitCycles	set	(_ClkOut/_BaudRate) + ((_ClkOut/4)/_BaudRate)
_tempCompute	set	(_SBitCycles >> 8)

_BIT1_INIT	set	08
SBitPrescale	set	0
SBitRtccLoad	set	_SBitCycles


 if (_tempCompute >= 1)
SBitPrescale	set	0
SBitRtccLoad	set	(_SBitCycles >> 1)
_BIT1_INIT	set	0
 endif

 if (_tempCompute >= 2)
SBitPrescale	set	1
SBitRtccLoad	set	(_SBitCycles >> 2)
 endif

 if (_tempCompute >= 4)
SBitPrescale	set	2
SBitRtccLoad	set	(_SBitCycles >> 3)
 endif

 if (_tempCompute >= 8)
SBitPrescale	set	3
SBitRtccLoad	set	(_SBitCycles >> 4)
 endif


 if (_tempCompute >= 16)
SBitPrescale	set	4
SBitRtccLoad	set	(_SBitCycles >> 5)
 endif


 if (_tempCompute >= 32)
SBitPrescale	set	5
SBitRtccLoad	set	(_SBitCycles >> 6)
 endif


 if (_tempCompute >= 64)
SBitPrescale	set	6
SBitRtccLoad	set	(_SBitCycles >> 7)
 endif


 if   (_tempCompute >= 128)
SBitPrescale	set	7
SBitRtccLoad	set	(_SBitCycles >> 8)
 endif

    

;
;*****************************************************************************************
;
#define	_Cycle_Offset1	24			;account for interrupt latency, call time

LOAD_RTCC	MACRO	Mode, K, Prescale

    if(UsePrescale == 0 && Mode == 0)
	movlw	-K + _Cycle_Offset1
    else
	movlw	-K + (_Cycle_Offset1 >> (Prescale+1))  ; Re Load RTCC init value + INT Latency Offset
    endif
	movwf	TMR0		; Note that Prescaler is cleared when RTCC is written

		ENDM
;*****************************************************************************************

LOAD_BITCOUNT	MACRO

	movlw	_DataBits+1
	movwf	BitCount
	movlw	1
	movwf	ExtraBitCount	

    if  _PARITY_ENABLE
	movlw	2
	movwf	ExtraBitCount
    endif

		ENDM
;
;**************************************************************************************************
;			Pin Assignements
;**************************************************************************************************
#define RX_MASK 0x10		; RX pin is connected to RA4, ie. bit 4
#define	RX_Pin	PORTA,4	; RX Pin : RA4
#define	RX	RxTemp,4

#define	TX	PORTB,7	; TX Pin , RB7

#define	_RTS	PORTB,5	; RTS Pin, RB5, Output signal
#define	_CTS	PORTB,6	; CTS Pin, RB6, Input signal


#define	_txmtProgress	SerialStatus,0
#define	_txmtEnable	SerialStatus,1

#define	_rcvProgress	SerialStatus,2
#define	_rcvOver	SerialStatus,3
#define	_ParityErr	SerialStatus,4
#define	_FrameErr	SerialStatus,5

#define	_parityBit	SerialStatus,7

;***************************************************************************************************

_OPTION_SBIT	set	0x38		; Increment on Ext Clock (falling edge), for START Bit Detect

  if UsePrescale
_OPTION_INIT	set	0x00		; Prescaler is used depending on Input Clock & Baud Rate
  else
_OPTION_INIT	set	0x0F
  endif

		CBLOCK	0x0C
			TxReg			; Transmit Data Holding/Shift Reg
			RxReg			; Rcv Data Holding Reg
                        RxTemp
			SerialStatus		; Txmt & Rev Status/Control Reg
			BitCount
			ExtraBitCount		; Parity & Stop Bit Count
			SaveWReg		; temp hold reg of WREG on INT
                        SaveStatus		; temp hold reg of STATUS Reg on INT
                    	temp1, temp2
		ENDC

;***************************************************************************************************

		LIST
