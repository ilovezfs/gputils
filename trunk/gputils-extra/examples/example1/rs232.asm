	TITLE           "RS232 Communications : Half Duplex : PIC16C6x/7x/8x"
	SUBTITLE        "Software Implementation : Interrupt Driven"

;*********************************************************************************************************
;                       Software Implementation Of RS232 Communications Using PIC16CXX
;                                              Half-Duplex 
;
;  These routines are intended to be used with PIC16C6X/7X family. These routines can be
;  used with processors in the 16C6X/7X family which do not have on board Hardware Async
;  Serial Port.
;  MX..
;
;  Description :
;               Half Duplex RS-232 Mode Is implemented in Software.
;               Both Reception & Transmission are Interrupt driven
;               Only 1 peripheral (RTCC) used for both transmission & reception
;               RTCC is used for both timing generation (for bit transmission & bit polling)
;               and Start Bit Detection in reception mode.
;               This is explained in more detail in the Interrupt Subroutine.
;               Programmable Baud Rate (speed depnding on Input Clock Freq.), programmable
;               #of bits, Parity enable/disable, odd/even parity is implemented.
;                       Parity & Framing errors are detected on Reception
;
;                               RS-232 Parameters
;
;The RS-232 Parameters are defined as shown below:
;
;               _ClkIn          :       Input Clock Frequency of the processor
;                                       (NOTE : RC Clock Mode Is Not Suggested due to wide variations)
;               _BaudRate       :       Desired Baud Rate. Any valid value can be used.
;                                       The highest Baud Rate achievable depends on Input Clock Freq.
;                                       300 to 4800 Baud was tested using 4 Mhz Input Clock
;                                       300 to 19200 Baud was tested using 10 Mhz Input Clock
;                                       Higher rates can be obtained using higher Input Clock Frequencies.
;                                       Once the _BaudRate & _ClkIn are specified the program
;                                       automatically selectes all the appropiate timings
;               _DataBits       :       Can specify 1 to 8 Bits.
;               _StopBits       :       Limited to 1 Stop Bit. Must set it to 1.
;               _PARITY_ENABLE  :       Parity Enable Flag. Set it to TRUE or FALSE. If PARITY
;                                       is used, then set it to TRUE, else FALSE. See "_ODD_PARITY" flag
;                                       description below
;               _ODD_PARITY     :       Set it to TRUE or FALSE. If TRUE, then ODD PARITY is used, else
;                                       EVEN Parity Scheme is used.
;                                       This Flag is ignored if _PARITY_ENABLE is set to FALSE.
;                
;
;  Usage :
;               An example is given in the main program on how to Receive & Transmit Data
;               In the example, the processor waits until a command is received. The command is interpreted
;               as the A/D Channel Number of PIC16C71. Upon reception of a command, the desired A/D channel
;               is selected and after A/D conversion, the 8 Bit A/D data is transmitted back to the Host.
;
;                       The RS-232 Control/Status Reg's bits are explained below :
;
;       "SerialStatus"          : RS-232 Status/Control Register
;
;       Bit 0   :       _txmtProgress   (1 if transmission in progress, 0 if transmission is complete)
;                                       After a byte is transmitted by calling "PutChar" function, the
;                                       user's code can poll this bit to check if transmission is complete.
;                                       This bit is reset after the STOP bit has been transmitted
;       Bit 1   :       _txmtEnable     Set this bit to 1 on initialization to enable transmission.
;                                       This bit can be used to Abort a transmission while the transmitter
;                                       is in progress (i.e when _txmtProgress = 1)
;       Bit 2   :       _rcvProgress    Indicates that the receiver is in middle of reception.It is reset when
;                                       a byte is received.
;       Bit 3   :       _rcvOver        This bit indicates the completion of Reception of a Byte. The user's
;                                       code can poll this bit after calling "GetChar" function. Once "GetChar"
;                                       function is called, this bit is 1 and is set to 0 after reception of
;                                       a complete byte (parity bit if enabled & stop bit)
;       Bit 4   :       _ParityErr      A 1 indicates Parity Error on Reception (for both even & odd parity) 
;       Bit 5   :       _FrameErr       A 1 indicates Framing Error On Reception
;
;       Bit 6   :       _unused_        Unimplemented Bit
;
;       Bit 7   :       _parityBit      The 9 th bit of transmission or reception (status of PARITY bit
;                                       if parity is enabled)
;
;       To Transmit A Byte Of Data :
;                       1) Make sure _txmtProgress & _rcvOver bits are cleared
;                       2) Load TxReg with data to be transmitted
;                       3) CALL  PutChar function                   
;
;       To Receive A Byte Of Data :
;                       1) Make sure _txmtProgress & _rcvOver bits are cleared
;                       2) CALL GetChar function
;                       3) The received Byte is in TxReg after _rcvOver bit is cleared
;
;
;
;       Program:          RS232.ASM 
;       Revision Date:   
;	                  May 17,1994 Scott Fink    (Rev 2)
;	                        	Corrected 7 bit and parity operation, corrected stop bit generation, corrected
;                       		receive prescaler settings.  Protected against inadvertant WDT reset.
;                         1-16-97      Compatibility with MPASMWIN 1.40
;
;*********************************************************************************************************

		Processor       16C71
		Radix   DEC
		EXPAND

		include         "p16c71.inc"

_ResetVector	set	0x00
_IntVector	set	0x04

TRUE	equ	1
FALSE	equ	0

;*********************************************************************************************************
;                               Setup RS-232 Parameters
;*********************************************************************************************************

_ClkIn          equ     4000000         ; Input Clock Frequency is 4 Mhz
_BaudRate       set     1200            ; Baud Rate (bits per second) is 1200
_DataBits       set     8               ; 8 bit data, can be 1 to 8
_StopBits       set     1               ; 1 Stop Bit, 2 Stop Bits is not implemented

#define _PARITY_ENABLE  FALSE           ; NO Parity
#define _ODD_PARITY     FALSE           ; EVEN Parity, if Parity enabled
#define _USE_RTSCTS     FALSE           ; NO Hardware Handshaking is Used

		include         "rs232.h"

;*********************************************************************************************************
;

	ORG     _ResetVector
	goto    Start
;
	
	ORG     _IntVector
	goto    Interrupt
;
;*********************************************************************************************************
;                               Table Of ADCON0 Reg
; Inputs : WREG (valid values are 0 thru 3)
; Returns In WREG, ADCON0 Value, selecting the desired Channel
;
;  Program Memory :     6 locations
;  Cycles         :     5
;
;*********************************************************************************************************

GetADCon0:
	andlw   0x03                    ; mask off all bits except 2 LSBs (for Channel # 0, 1, 2, 3)
	addwf   PCL, F
	retlw   (0xC1 | (0 << 3))       ; channel 0
	retlw   (0xC1 | (1 << 3))       ; channel 1
	retlw   (0xC1 | (2 << 3))       ; channel 2
GetADCon0_End:
	retlw   (0xC1 | (3 << 3))       ; channel 3

   if( (GetADCon0 & 0xff) >= (GetADCon0_End & 0xff))
	MESSG   "Warning : Crossing Page Boundary in Computed Jump, Make Sure PCLATH is Loaded Correctly"
   endif
;
;*********************************************************************************************************
;                               Initialize A/D Converter
;  <RA0:RA3>    Configure as Analog Inputs, VDD as Vref
;   A/D Clock Is Internal RC Clock
;   Select Channel 0
;
;  Program Memory :     6 locations
;  Cycles         :     7
;
;*********************************************************************************************************
InitAtoD:
	bsf     STATUS,RP0
	clrf    ADCON1
	bcf     STATUS,RP0
	movlw   0xC1
	movwf   ADCON0
	return
;
;*********************************************************************************************************
;                                       Main Program Loop
;
;  After appropriate initilization, The main program wait for a command from RS-232
;  The command is 0, 1, 2 or 3. This command/data represents the A/D Channel Number.
;  After a command is received, the appropriate A/D Channel is seleted and when conversion is
;  completed the A/D Data is transmitted back to the Host. The controller now waits for a new
;  command.  
;*********************************************************************************************************

Start:
	call    InitSerialPort
;
WaitForNextSel:
   if _USE_RTSCTS
	bcf     STATUS,RP0
	bcf     _RTS                    ; ready to accept data from host
   endif
	call    GetChar                 ; wait for a byte reception
	btfsc   _rcvOver                ; _rcvOver Gets Cleared when a Byte Is Received (in RxReg)
	goto    $-1                     ; USER can perform other jobs here, can poll _rcvOver bit
;
; A Byte is received, Select The Desired Channel & TMXT the desired A/D Channel Data
;
	bcf     STATUS,RP0                    ; make sure to select Page 0
	movf    RxReg,W                 ; WREG = Commanded Channel # (0 thru 3)
	call    GetADCon0               ; Get ADCON0 Reg Constant from Table Lookup
	movwf   ADCON0                 ; Load ADCON0 reg, selecting the desired channel
	nop
;
	bsf     ADCON0,GO_DONE                     ; start conversion
	btfsc   ADCON0,GO_DONE
	goto    $-1                     ; Loop Until A/D Conversion Done

	movf    ADRES,W
	movwf   TxReg
   if _USE_RTSCTS
	bsf     _RTS                    ; Half duplex mode, transmission mode, ask host not to send data
	btfsc   _CTS                    ; Check CTS signal if host ready to accept data
	goto    $-1
   endif
	call    PutChar
	btfsc   _txmtProgress
	goto    $-1                     ; Loop Until Transmission Over, User Can Perform Other Jobs


;
	goto    WaitForNextSel          ; wait for next selection (command from Serial Port)
;
;*********************************************************************************************************
;                               RS-232 Routines
;;*********************************************************************************************************
;                               Interrupt Service Routine
;
; Only RTCC Inturrupt Is used. RTCC Inturrupt is used as timing for Serial Port Receive & Transmit
; Since RS-232 is implemented only as a Half Duplex System, The RTCC is shared by both Receive &
; Transmit Modules.
;       Transmission :
;                       RTCC is setup for Internal Clock increments and interrupt is generated when
;                       RTCC overflows. Prescaler is assigned, depending on The INPUT CLOCK & the
;                       desired BAUD RATE.
;       Reception :
;                       When put in receive mode, RTCC is setup for external clock mode (FALLING EDGE)
;                       and preloaded with 0xFF. When a Falling Edge is detected on RTCC Pin, RTCC 
;                       rolls over and an Interrupt is generated (thus Start Bit Detect). Once the start
;                       bit is detected, RTCC is changed to INTERNAL CLOCK mode and RTCC is preloaded
;                       with a certain value for regular timing interrupts to Poll RTCC Pin (i.e RX pin). 
;
;*********************************************************************************************************

Interrupt:
	btfss   INTCON,T0IF
	retfie                          ; other interrupt, simply return & enable GIE
;
; Save Status On INT : WREG & STATUS Regs
;
	movwf   SaveWReg
	swapf   STATUS,W               ; affects no STATUS bits : Only way OUT to save STATUS Reg ?????
	movwf   SaveStatus
;
	btfsc   _txmtProgress
	goto    _TxmtNextBit            ; Txmt Next Bit
	btfsc   _rcvProgress
	goto    _RcvNextBit             ; Receive Next Bit
	goto    _SBitDetected           ; Must be start Bit     
;
RestoreIntStatus:
	swapf   SaveStatus,W
	movwf   STATUS                 ; restore STATUS Reg
	swapf   SaveWReg, F             ; save WREG
	swapf   SaveWReg,W              ; restore WREG
	bcf     INTCON,T0IF
	retfie
;
;*********************************************************************************************************
;  
;  
;
; Configure TX Pin as output, make sure TX Pin Comes up in high state on Reset
; Configure, RX_Pin (RTCC pin) as Input, which is used to poll data on reception
;
;  Program Memory :     9 locations
;  Cycles         :     10
;*********************************************************************************************************
						
InitSerialPort:
	clrf    SerialStatus
;
	bcf     STATUS,RP0                            ; select Page 0 for Port Access
	bsf     TX                              ; make sure TX Pin is high on powerup, use RB Port Pullup                       
	bsf     STATUS,RP0                            ; Select Page 1 for TrisB access
	bcf     TX                              ; set TX Pin As Output Pin, by modifying TRIS
  if _USE_RTSCTS
	bcf     _RTS                            ; RTS is output signal, controlled by PIC16Cxx
	bsf     _CTS                            ; CTS is Input signal, controlled by the host
  endif
	bsf     RX_Pin                          ; set RX Pin As Input for reception
	return
;
;*********************************************************************************************************

	include "txmtr.asm"             	; The Transmit routines are in file "txmtr.asm"
	include "rcvr.asm"              	; The Receiver Routines are in File "rcvr.asm"

;*********************************************************************************************************

	END




