
;*********************************************************************
;                                                                    *
;                       Software License Agreement                   *
;                                                                    *
;   The software supplied herewith by Microchip Technology           *
;   Incorporated (the "Company") for its PICmicro® Microcontroller   *
;   is intended and supplied to you, the Company’s customer, for use *
;   solely and exclusively on Microchip PICmicro Microcontroller     *
;   products. The software is owned by the Company and/or its        *
;   supplier, and is protected under applicable copyright laws. All  *
;   rights are reserved. Any use in violation of the foregoing       *
;   restrictions may subject the user to criminal sanctions under    *
;   applicable laws, as well as to civil liability for the breach of *
;   the terms and conditions of this license.                        *
;                                                                    *
;   THIS SOFTWARE IS PROVIDED IN AN "AS IS" CONDITION.  NO           *
;   WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING,    *
;   BUT NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND    *
;   FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE     *
;   COMPANY SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL,  *
;   INCIDENTAL OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.  *
;                                                                    *
;*********************************************************************


;*********************************************************************
;                                                                    *
;     Implementing Master I2C with the MSSP module on a PICmicro     *
;                                                                    *
;*********************************************************************
;                                                                    *
;   Filename:       i2ccomm.asm                                      *
;   Date:           07/18/2000                                       *
;   Revision:       1.00                                             *
;                                                                    *
;   Tools:          MPLAB   5.11.00                                  *
;                   MPLINK  2.10.00                                  *
;                   MPASM   2.50.00                                  *
;                                                                    *
;  Author:          Richard L. Fischer                               *
;                   John E. Andrews                                  *
;                                                                    *
;  Company:         Microchip Technology Incorporated                *
;                                                                    *
;*********************************************************************
;                                                                    *
;   Files required:                                                  *
;                                                                    *
;                   i2ccomm.asm                                      *
;                                                                    *
;                   i2ccomm.inc                                      *
;                   flags.inc    (referenced in i2ccomm.inc file)    *
;                   i2ccomm1.inc (must be included in main file)     *
;                   p16f873.inc                                      *          
;                                                                    *
;*********************************************************************
;                                                                    *
;   Notes:   The routines within this file are used to read from     *
;   and write to a Slave I2C device. The MSSP initialization         *
;   function is also contained within this file.                     *     
;                                                                    *
;********************************************************************/

	#include <p16f873.inc>               ; processor specific definitions
	#include  "i2ccomm.inc"              ; required include file
	errorlevel -302	

 

#define  FOSC        D'8000000'          ; define FOSC to PICmicro
#define  I2CClock    D'400000'           ; define I2C bite rate
#define  ClockValue  (((FOSC/I2CClock)/4) -1) ; 

	

;----------------------------------------------------------------------
;   ***********************  I2C Service  *************************
;----------------------------------------------------------------------
I2C_COMM	CODE	
service_i2c

		movlw	high  I2CJump	         ; fetch upper byte of jump table address
		movwf	PCLATH				     ; load into upper PC latch
		movlw	i2cSizeMask
		banksel i2cState                 ; select GPR bank
		andwf	i2cState,w               ; retrieve current I2C state
		addlw	low  (I2CJump + 1)       ; calc state machine jump addr into W		
		btfsc	STATUS,C                 ; skip if carry occured
		incf	PCLATH,f		         ; otherwise add carry
I2CJump	; address were jump table branch occurs, this addr also used in fill 
		movwf	PCL  		             ; index into state machine jump table
		; jump to processing for each state		= i2cState value for each state

		goto	WrtStart                 ; write start sequence           =  0
		goto	SendWrtAddr              ; write address, R/W=1           =  1
		goto	WrtAckTest               ; test ack,write data            =  2
     	goto 	WrtStop                  ; do stop if done                =  3

		goto	ReadStart                ; write start sequence           =  4
		goto	SendReadAddr             ; write address, R/W=0           =  5
		goto    ReadAckTest              ; test acknowledge after address =  6
 		goto	ReadData                 ; read more data                 =  7
		goto	ReadStop                 ; generate stop sequence         =  8

I2CJumpEnd		 
        Fill (return),  (I2CJump-I2CJumpEnd) + i2cSizeMask 




;----------------------------------------------------------------------
;   ********************* Write data to Slave   *********************
;----------------------------------------------------------------------
; Generate I2C bus start condition               [ I2C STATE -> 0 ]
WrtStart
		banksel	write_ptr                ; select GPR bank
		movf	write_ptr,w              ; retrieve ptr address
		movwf	FSR                      ; initialize FSR for indirect access
		incf	i2cState,f               ; update I2C state variable
		banksel SSPCON2                  ; select SFR bank
		bsf     SSPCON2,SEN              ; initiate I2C bus start condition
		return                           ; 

; Generate I2C address write (R/W=0)             [ I2C STATE -> 1 ]
SendWrtAddr
		banksel temp_address             ; select GPR bank
		bcf		STATUS,C                 ; ensure carry bit is clear
		rlf		temp_address,w           ; compose 7-bit address
		incf	i2cState,f               ; update I2C state variable
		banksel	SSPBUF                   ; select SFR bank
		movwf   SSPBUF                   ; initiate I2C bus write condition
		return                           ; 

; Test acknowledge after address and data write  [ I2C STATE -> 2 ]
WrtAckTest
		banksel SSPCON2                  ; select SFR bank
		btfss   SSPCON2,ACKSTAT          ; test for acknowledge from slave
		goto	WrtData                  ; go to write data module
		banksel eflag_event              ; select GPR bank
		bsf		eflag_event,ack_error    ; set acknowledge error
		clrf	i2cState                 ; reset I2C state variable
		banksel	SSPCON2                  ; select SFR bank
		bsf     SSPCON2,PEN              ; initiate I2C bus stop condition
		return                           ;

; Generate I2C write data condition
WrtData
		movf    INDF,w                   ; retrieve byte into w
		banksel	write_count              ; select GPR bank
		decfsz	write_count,f            ; test if all done with writes
		goto	send_byte                ; not end of string
		incf	i2cState,f               ; update I2C state variable
send_byte
		banksel SSPBUF                   ; select SFR bank
		movwf   SSPBUF                   ; initiate I2C bus write condition
		incf	FSR,f                    ; increment pointer
		return                           ; 


; Generate I2C bus stop condition                [ I2C STATE -> 3 ]
WrtStop
		banksel SSPCON2                  ; select SFR bank
		btfss   SSPCON2,ACKSTAT          ; test for acknowledge from slave
		goto	no_error                 ; bypass setting error flag
		banksel eflag_event              ; select GPR bank
		bsf		eflag_event,ack_error    ; set acknowledge error
		clrf	i2cState                 ; reset I2C state variable
		goto	stop
no_error
		banksel i2cState                 ; select GPR bank
		incf	i2cState,f               ; update I2C state variable for read
stop
		banksel	SSPCON2                  ; select SFR bank
		bsf     SSPCON2,PEN              ; initiate I2C bus stop condition
		return                           ;




;----------------------------------------------------------------------
;   ********************* Read data from Slave   *********************
;----------------------------------------------------------------------
; Generate I2C start condition                   [ I2C STATE -> 4 ]
ReadStart
		banksel	read_ptr                 ; select GPR bank
		movf	read_ptr,W               ; retrieve ptr address
		movwf	FSR                      ; initialize FSR for indirect access
		incf	i2cState,f               ; update I2C state variable
		banksel SSPCON2                  ; select SFR bank
		bsf     SSPCON2,SEN              ; initiate I2C bus start condition
		return                           ; 

; Generate I2C address write (R/W=1)             [ I2C STATE -> 5 ]
SendReadAddr 
		banksel temp_address             ; select GPR bank
		bsf		STATUS,C                 ; ensure cary bit is clear
		rlf		temp_address,w           ; compose 7 bit address
		incf	i2cState,f               ; update I2C state variable
		banksel	SSPBUF                   ; select SFR bank
		movwf   SSPBUF                   ; initiate I2C bus write condition
		return                           ; 

; Test acknowledge after address write           [ I2C STATE -> 6 ]
ReadAckTest
		banksel SSPCON2                  ; select SFR bank
		btfss   SSPCON2,ACKSTAT          ; test for not acknowledge from slave
		goto	StartReadData            ; good ack, go issue bus read
		banksel eflag_event              ; ack error, so select GPR bank
		bsf		eflag_event,ack_error    ; set ack error flag
		clrf	i2cState                 ; reset I2C state variable
		banksel	SSPCON2                  ; select SFR bank
		bsf     SSPCON2,PEN              ; initiate I2C bus stop condition
		return

StartReadData
		bsf		SSPCON2,RCEN             ; generate receive condition
		banksel i2cState                 ; select GPR bank
		incf	i2cState,f               ; update I2C state variable
		return

; Read slave I2C                                 [ I2C STATE -> 7 ]
ReadData
		banksel SSPBUF                   ; select SFR bank
		movf    SSPBUF,w                 ; save off byte into W
		banksel	read_count               ; select GPR bank
		decfsz	read_count,f             ; test if all done with reads
		goto	SendReadAck 		     ; not end of string so send ACK
		

; Send Not Acknowledge
SendReadNack
		movwf   INDF                     ; save off null character
		incf	i2cState,f               ; update I2C state variable
		banksel	SSPCON2                  ; select SFR bank
		bsf     SSPCON2,ACKDT            ; acknowledge bit state to send (not ack)
		bsf     SSPCON2,ACKEN            ; initiate acknowledge sequence
		return

; Send Acknowledge
SendReadAck
		movwf   INDF                     ; no, save off byte
		incf	FSR,f                    ; update receive pointer
		banksel	SSPCON2                  ; select SFR bank
		bcf     SSPCON2,ACKDT            ; acknowledge bit state to send
		bsf     SSPCON2,ACKEN            ; initiate acknowledge sequence
		btfsc	SSPCON2,ACKEN            ; ack cycle complete?
		goto	$-1                      ; no, so loop again
		bsf		SSPCON2,RCEN             ; generate receive condition
		return                           ; 


; Generate I2C stop condition                    [ I2C STATE -> 8 ]
ReadStop
		banksel	SSPCON2                  ; select SFR bank
		bcf		PIE1,SSPIE               ; disable SSP interrupt
		bsf     SSPCON2,PEN              ; initiate I2C bus stop condition
		banksel i2cState                 ; select GPR bank
		clrf	i2cState                 ; reset I2C state variable
		bsf		sflag_event,rw_done      ; set read/write done flag
		return 


	
;----------------------------------------------------------------------
;   ******************* Generic bus idle check ***********************
;----------------------------------------------------------------------
; test for i2c bus idle state; not implemented in this code (example only)
i2c_idle   
		banksel SSPSTAT                  ; select SFR bank
		btfsc   SSPSTAT,R_W              ; test if transmit is progress 
		goto    $-1                      ; module busy so wait
		banksel SSPCON2                  ; select SFR bank
		movf    SSPCON2,w                ; get copy of SSPCON2 for status bits
		andlw   0x1F                     ; mask out non-status bits
		btfss   STATUS,Z                 ; test for zero state, if Z set, bus is idle
		goto    $-3                      ; bus is busy so test again
		return                           ; return to calling routine




;----------------------------------------------------------------------
;  ******************* INITIALIZE MSSP MODULE  *******************
;----------------------------------------------------------------------

init_i2c
		banksel SSPADD                 ; select SFR bank
		movlw   ClockValue             ; read selected baud rate 
		movwf   SSPADD                 ; initialize I2C baud rate
		bcf     SSPSTAT,6              ; select I2C input levels
		bcf     SSPSTAT,7              ; enable slew rate

		movlw	b'00011000'            ;
		iorwf	TRISC,f                ; ensure SDA and SCL are inputs
		bcf		STATUS,RP0             ; select SFR bank
		movlw   b'00111000'            ; 
		movwf   SSPCON                 ; Master mode, SSP enable
		return                         ; return from subroutine


		END                            ; required directive

