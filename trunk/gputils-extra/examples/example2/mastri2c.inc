
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
;   Filename:       mastri2c.inc                                     *
;   Date:           07/18/2000                                       *
;   Revision:       1.00                                             *
;                                                                    *
;   Tools:          MPLAB   5.11.00                                  *
;                   MPLINK  2.10.00                                  *
;                   MPASM   2.50.00                                  *
;                                                                    *
;*********************************************************************

	

;*******    INTERRUPT CONTEXT SAVE/RESTORE VARIABLES
INT_VAR        UDATA   0x20              ; create uninitialized data "udata" section
w_temp           RES     1               ;
status_temp      RES     1               ;
pclath_temp      RES     1


INT_VAR1       UDATA   0xA0              ; reserve location 0xA0
w_temp1          RES     1


;*******    GENERAL PURPOSE VARIABLES
GPR_DATA		UDATA
temp_hold        RES     1               ; temp variable for string compare
ptr1             RES     1               ; used as pointer in string compare 
ptr2             RES     1               ; used as pointer in string compare


STRING_DATA		 UDATA	
write_string     RES      D'30'
read_string      RES      D'30'       



		EXTERN		init_timer1          ; reference linkage for function
		EXTERN		init_ports           ; reference linkage for function

