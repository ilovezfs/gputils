
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
;   Filename:       mastri2c.asm                                     *
;   Date:           07/18/2000                                       *
;   Revision:       1.00                                             *
;                                                                    *
;   Tools:          MPLAB   5.11.00                                  *
;                   MPLINK  2.10.00                                  *
;                   MPASM   2.50.00                                  *
;                                                                    *
;  Author:          Richard L. Fischer                               *
;                                                                    *
;  Company:         Microchip Technology Incorporated                *
;                                                                    *
;*********************************************************************
;                                                                    *
;   System files required:                                           *
;                                                                    *
;                   mastri2c.asm                                     *
;                   i2ccomm.asm                                      *
;                   init.asm                                         *
;                                                                    *
;                   mastri2c.inc                                     *
;                   i2ccomm.inc                                      *
;                   i2ccomm1.inc                                     *
;                   flags.inc                                        *
;                                                                    *
;                   p16f873.inc                                      *
;                   16f873.lkr     (modified for interrupts)         *
;                                                                    *
;*********************************************************************
;                                                                    *
;   Notes:                                                           *
;	                                                                 *
;   Device Fosc -> 8.00MHz                                           *
;	WDT -> on                                                        *
;	Brownout -> on                                                   *
;	Powerup timer -> on                                              *
;	Code Protect -> off                                              *
;                                                                    *
;   Interrupt sources -                                              *
;                       1. I2C events (valid events)                 *
;                       2. I2C Bus Collision                         *
;                       3. Timer1 - 100mS intervals                  *
;                                                                    *
;                                                                    * 
;                                                                    *
;********************************************************************/

	list      p=16f873                   ; list directive to define processor
	#include <p16f873.inc>               ; processor specific variable definitions
	__CONFIG (_CP_OFF & _WDT_ON & _BODEN_ON & _PWRTE_ON & _HS_OSC & _WRT_ENABLE_ON & _LVP_OFF & _CPD_OFF)

	#include  "mastri2c.inc"             ;
	#include  "i2ccomm1.inc"             ; required include file

	errorlevel -302	



#define  ADDRESS     0x01                ; Slave I2C address




;----------------------------------------------------------------------
;   ********************* RESET VECTOR LOCATION  ********************
;----------------------------------------------------------------------
RESET_VECTOR  CODE    0x000              ; processor reset vector
		movlw  high  start               ; load upper byte of 'start' label
		movwf  PCLATH                    ; initialize PCLATH
		goto   start                     ; go to beginning of program


;----------------------------------------------------------------------
;  ******************* INTERRUPT VECTOR LOCATION  *******************
;----------------------------------------------------------------------
INT_VECTOR   CODE    0x004               ; interrupt vector location
		movwf   w_temp                   ; save off current W register contents
		movf	STATUS,w                 ; move status register into W register
		clrf	STATUS                   ; ensure file register bank set to 0
		movwf	status_temp              ; save off contents of STATUS register
		movf	PCLATH,w
		movwf	pclath_temp              ; save off current copy of PCLATH
		clrf	PCLATH		             ; reset PCLATH to page 0
	

; TEST FOR COMPLETION OF VALID I2C EVENT 
		bsf		STATUS,RP0               ; select SFR bank
		btfss	PIE1,SSPIE               ; test is interrupt is enabled
		goto	test_buscoll             ; no, so test for Bus Collision Int
		bcf		STATUS,RP0               ; select SFR bank
		btfss   PIR1,SSPIF               ; test for SSP H/W flag
		goto	test_buscoll             ; no, so test for Bus Collision Int
		bcf		PIR1,SSPIF               ; clear SSP H/W flag

		pagesel	service_i2c              ; select page bits for function
		call    service_i2c              ; service valid I2C event		


; TEST FOR I2C BUS COLLISION EVENT
test_buscoll 
		banksel PIE2                     ; select SFR bank
		btfss	PIE2,BCLIE               ; test if interrupt is enabled
		goto	test_timer1              ; no, so test for Timer1 interrupt
		bcf		STATUS,RP0               ; select SFR bank
		btfss   PIR2,BCLIF               ; test if Bus Collision occured
		goto    test_timer1              ; no, so test for Timer1 interrupt
		bcf		PIR2,BCLIF               ; clear Bus Collision H/W flag

		call	service_buscoll          ; service bus collision error


; TEST FOR TIMER1 ROLLOVER EVENT		
test_timer1
		banksel PIE1                     ; select SFR bank
		btfss	PIE1,TMR1IE              ; test if interrupt is enabled
		goto	exit_isr                 ; no, so exit ISR
		bcf		STATUS,RP0               ; select SFR bank
		btfss   PIR1,TMR1IF              ; test if Timer1 rollover occured
		goto    exit_isr                 ; no so exit isr
		bcf		PIR1,TMR1IF              ; clear Timer1 H/W flag

		pagesel	service_i2c              ; select page bits for function
 		call    service_i2c              ; service valid I2C event
		banksel	T1CON                    ; select SFR bank
		bcf		T1CON,TMR1ON             ; turn off Timer1 module
		movlw	0x58                     ;
		addwf	TMR1L,f                  ; reload Timer1 low
		movlw	0x9E                     ;
		movwf	TMR1H                    ; reload Timer1 high
		banksel PIE1                     ; select SFR bank
		bcf		PIE1,TMR1IE              ; disable Timer1 interrupt 
		bsf		PIE1,SSPIE               ; enable SSP H/W interrupt

exit_isr 
		clrf	STATUS                   ; ensure file register bank set to 0
		movf	pclath_temp,w
		movwf	PCLATH                   ; restore PCLATH
		movf    status_temp,w            ; retrieve copy of STATUS register
		movwf	STATUS                   ; restore pre-isr STATUS register contents
		swapf   w_temp,f                 ;
		swapf   w_temp,w                 ; restore pre-isr W register contents
		retfie                           ; return from interrupt



;----------------------------------------------------------------------
;   ******************* MAIN CODE START LOCATION  ******************
;----------------------------------------------------------------------
MAIN    CODE
start
     	pagesel	init_ports               ; 
		call	init_ports               ; initialize Ports
		call	init_timer1              ; initialize Timer1
		pagesel	init_i2c
		call    init_i2c                 ; initialize I2C module

		banksel eflag_event              ; select GPR bank
		clrf    eflag_event              ; initialize event flag variable
		clrf    sflag_event              ; initialize event flag variable
		clrf	i2cState                 ;

		call	CopyRom2Ram              ; copy ROM string to RAM

		call	init_vars                ; initialize variables
	
		banksel PIE2                     ; select SFR bank
		bsf		PIE2,BCLIE               ; enable interrupt
		banksel PIE1                     ; select SFR bank
		bsf		PIE1,TMR1IE              ; enable Timer1 interrupt

		bsf     INTCON,PEIE              ; enable peripheral interrupt
		bsf		INTCON,GIE               ; enable global interrupt



;*******************************************************************
;                     MAIN LOOP BEGINS HERE
;******************************************************************* 
main_loop 
		clrwdt                           ; reset WDT

		banksel eflag_event              ; select SFR bank
		btfsc	eflag_event,ack_error    ; test for ack error event flag
		call	service_ackerror         ; service ack error

 		banksel sflag_event              ; select SFR bank
 		btfss	sflag_event,rw_done      ; test if read/write cycle complete
		goto    main_loop                ; goto main loop 
		call	string_compare           ; else, go compare strings

		banksel T1CON                    ; select SFR bank
		bsf		T1CON,TMR1ON             ; turn on Timer1 module
		banksel PIE1                     ; select SFR bank
		bsf		PIE1,TMR1IE              ; re-enable Timer1 interrupts

		call	init_vars                ; re-initialize variables
		goto    main_loop                ; goto main loop 




;----------------------------------------------------------------------
;   *************** Bus Collision Service Routine ******************
;----------------------------------------------------------------------
service_buscoll  
		banksel i2cState                 ; select GPR bank
		clrf	i2cState                 ; reset I2C bus state variable
		call	init_vars                ; re-initialize variables
		bsf		T1CON,TMR1ON             ; turn on Timer1 module
		banksel PIE1                     ; select SFR bank
		bsf		PIE1,TMR1IE              ; enable Timer1 interrupt
		return                           ;


;----------------------------------------------------------------------
;   ************* Acknowledge Error Service Routine ***************
;----------------------------------------------------------------------
service_ackerror
		banksel eflag_event              ; select SFR bank
		bcf		eflag_event,ack_error    ; reset acknowledge error event flag
		clrf	i2cState                 ; reset bus state variable
		call	init_vars                ; re-initialize variables
		bsf		T1CON,TMR1ON             ; turn on Timer1 module
		banksel PIE1                     ; select SFR bank
		bsf		PIE1,TMR1IE              ; enable Timer1 interrupt 
		return                           ;
	


;----------------------------------------------------------------------
;   *****  INITIALIZE VARIABLES USED IN SERVICE_I2C FUNCTION  ******
;----------------------------------------------------------------------
init_vars
		movlw	D'21'                    ; byte count for this example
		banksel write_count              ; select GPR bank
		movwf	write_count              ; initialize write count
		movwf	read_count               ; initialize read count

		movlw	write_string             ; get write string array address
		movwf	write_ptr                ; initialize write pointer

		movlw	read_string              ; get read string placement address
		movwf	read_ptr                 ; initialize read pointer

		movlw	ADDRESS                  ; get address of slave
		movwf	temp_address             ; initialize temporary address hold reg
		return                           ;


;----------------------------------------------------------------------
;  *******************  Compare Strings   ************************
;----------------------------------------------------------------------
;Compare the string written to and read back from the Slave
string_compare
		movlw	read_string              ; 
		banksel ptr1                     ; select GPR bank
		movwf	ptr1                     ; initialize first pointer
		movlw	write_string
		movwf	ptr2                     ; initialize second pointer

loop	movf	ptr1,w                   ; get address of first pointer
		movwf	FSR                      ; init FSR
		movf	INDF,w                   ; retrieve one byte
		banksel temp_hold                ; select GPR bank
		movwf	temp_hold                ; save off byte 1
		movf	ptr2,w                   ;
		movwf	FSR                      ; init FSR
		movf	INDF,w                   ; retrieve second byte
		subwf	temp_hold,f              ; do comparison
		btfss	STATUS,Z                 ; test for valid compare
		goto	not_equal                ; bytes not equal
		iorlw	0x00                     ; test for null character
		btfsc	STATUS,Z                 ;
		goto	end_string               ; end of string has been reached
		incf	ptr1,f                   ; update first pointer
		incf	ptr2,f                   ; update second pointer
		goto	loop                     ; do more comparisons

not_equal
		banksel	PORTB                    ; select GPR bank
		movlw	b'00000001'
		xorwf	PORTB,f
		goto	exit
end_string
		banksel	PORTB                    ; select GPR bank
		movlw	b'00000010'              ; no error
		xorwf	PORTB,f
exit
		banksel sflag_event              ; select SFR bank
		bcf		sflag_event,rw_done      ; reset flag
		return



;----------------------------------------------------------------------
;  *******************  Program Memory Read   *******************
;----------------------------------------------------------------------
;Read the message from location MessageTable
CopyRom2Ram
		movlw	write_string             ;
		movwf	FSR                      ; initialize FSR

		banksel	EEADRH                   ; select SFR bank
		movlw	High (Message1)          ; point to the Message Table
		movwf	EEADRH		             ; init SFR EEADRH
		movlw	Low (Message1)           ; 
		movwf	EEADR                    ; init SFR EEADR

next1
		banksel EECON1                   ; select SFR bank
		bsf		EECON1,EEPGD             ; select the program memory
		bsf		EECON1,RD                ; read word
		nop                              ;
		nop                              ;
		banksel EEDATA                   ;
		rlf		EEDATA,w                 ; get bit 7 in carry
		rlf		EEDATH,w                 ; get high byte in w

		movwf	INDF                     ; save it
		incf	FSR,f                    ;
	
		banksel	EEDATA                   ; select SFR bank
		bcf		EEDATA,7                 ; clr bit 7
		movf	EEDATA,w                 ; get low byte and see = 0?
		btfsc	STATUS,Z                 ; end?
		return                           ;
		movwf	INDF                     ; save it
		incf	FSR,f                    ; update FSR pointer 
		banksel	EEADR                    ; point to address
		incf	EEADR,f                  ; inc to next location
		btfsc	STATUS,Z                 ; cross over 0xff
		incf	EEADRH,f                 ; yes then inc high
		goto	next1	                 ; read next byte 



;----------------------------------------------------------------------
;----------------------------------------------------------------------

Message1		DA      "Master and Slave I2C",0x00,0x00


		END                              ; required directive

