

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
;   Filename:      init.asm                                          *
;   Date:          07/18/2000                                        *
;   Revision:      1.00                                              *
;                                                                    *
;   Tools:         MPLAB   5.11.00                                   *
;                  MPLINK  2.10.00                                   *
;                  MPASM   2.50.00                                   *
;                                                                    *
;  Author:         Richard L. Fischer                                *
;                                                                    *
;  Company:        Microchip Technology Incorporated                 *
;                                                                    *
;*********************************************************************
;                                                                    *
;   Files required:                                                  *
;                                                                    *
;                   init.asm                                         *
;                                                                    *
;                   p16f873.inc                                      *
;                                                                    *
;                                                                    *
;*********************************************************************
;                                                                    *
;   Notes:                                                           *
;                                                                    *
;                                                                    *
;********************************************************************/

	#include <p16f873.inc>             ; processor specific variable definitions
	errorlevel -302	



	
		GLOBAL	init_timer1            ; make function viewable for other modules
		GLOBAL	init_ports             ; make function viewable for other modules




;----------------------------------------------------------------------
;  ******************* INITIALIZE PORTS  *************************
;----------------------------------------------------------------------
INIT_CODE		CODE

init_ports
		banksel	PORTA                  ; select SFR bank
		clrf	PORTA                  ; initialize PORTS
		clrf	PORTB                  ; 
		clrf	PORTC                  ;

		bsf		STATUS,RP0             ; select SFR bank
		movlw	b'00000110'            ;
		movwf	ADCON1                 ; make PORTA digital
		clrf	TRISB                  ;
		movlw	b'000000'              ;  
		movwf	TRISA                  ;
		movlw	b'00011000'            ;
		movwf	TRISC                  ;
		return



;----------------------------------------------------------------------
;  ******************* INITIALIZE TIMER1 MODULE  *******************
;----------------------------------------------------------------------
init_timer1
		banksel T1CON                  ; select SFR bank	
		movlw   b'00110000'            ; 1:8 prescale, 100mS rollover
		movwf	T1CON                  ; initialize Timer1

		movlw	0x58                   ;
		movwf	TMR1L                  ; initialize Timer1 low
		movlw	0x9E                   ;
		movwf	TMR1H                  ; initialize Timer1 high

		bcf		PIR1,TMR1IF            ; ensure flag is reset
		bsf		T1CON,TMR1ON           ; turn on Timer1 module
		return                         ; return from subroutine

		END                            ; required directive
