
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
;   Filename:       flags.inc                                        *
;   Date:           07/18/2000                                       *
;   Revision:       1.00                                             *
;                                                                    *
;   Tools:          MPLAB   5.11.00                                  *
;                   MPLINK  2.10.00                                  *
;                   MPASM   2.50.00                                  *
;                                                                    *
;*********************************************************************
;                                                                    *
;   Notes:                                                           *
;	                                                                 *
;	This file defines the flags used in the i2ccomm.asm file.        *
;                                                                    * 
;                                                                    *
;********************************************************************/


 
; bits for variable sflag_event
#define  sh1         0                   ; place holder
#define  sh2         1                   ; place holder
#define  sh3         2                   ; place holder
#define  sh4         3                   ; place holder
#define  sh5         4                   ; place holder
#define  sh6         5                   ; place holder
#define  sh7         6                   ; place holder
#define  rw_done     7                   ; flag bit


; bits for variable eflag_event
#define  ack_error   0                   ; flag bit
#define  eh1         1                   ; place holder
#define  eh2         2                   ; place holder
#define  eh3         3                   ; place holder
#define  eh4         4                   ; place holder
#define  eh5         5                   ; place holder
#define  eh6         6                   ; place holder
#define  eh7         7                   ; place holder
