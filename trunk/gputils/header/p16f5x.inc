	LIST
; P16F5X.INC  Standard Header File, Version 4.00    Microchip Technology, Inc.
	NOLIST

; This header file defines configurations, registers, and other useful bits of
; information for the 16F5X microcontrollers.  These names are taken to match 
; the data sheets as closely as possible.  The microcontrollers included
; in this file are:

;		16F57
;		16F54


; There is one group of symbols that is valid for all microcontrollers.  
; Each microcontroller in this family also has its own section of special 
; symbols.  Note that the processor must be selected before this file is 
; included.  The processor may be selected the following ways:

;       1. Command line switch:
;               C:\ MPASM MYFILE.ASM /P16C54A
;       2. LIST directive in the source file
;               LIST   P=16C54A
;       3. Processor Type entry in the MPASM full-screen interface

;==========================================================================
;
;	   Revision History
;
;==========================================================================

;Rev:   Date:   Reason:

;5.00	06/09/03 Initial modification for the PIC16F5X series fo parts
;4.12   12/02/02 Added 16CR57C
;4.11   01/18/99 Added 54C, 55A, 57C for interim release
;4.10   08/18/98 Added new processor for MPASM v2.16
;4.00   06/05/97 Added new processors for MPASM v2.00
;3.30   07/16/96 Aligned processors with MPASM v1.40
;3.20	04/09/96 Added 16C54B, 16CR56B, 16C58B
;3.10   12/14/95 Added 16C52
;3.01   11/29/95 Removed 16CR55
;3.00   10/16/95 Added new processors for MPASM v1.30
;2.04   07/26/95 Reformatted for readability
;2.03   06/21/95 Removed leading spaces


;==========================================================================
;
;       Generic Definitions
;
;==========================================================================

W                            EQU     H'0000'
F                            EQU     H'0001'

;----- Register Files -----------------------------------------------------

INDF                         EQU     H'0000'
TMR0                         EQU     H'0001'
PCL                          EQU     H'0002'
STATUS                       EQU     H'0003'
FSR                          EQU     H'0004'
PORTA                        EQU     H'0005'
PORTB                        EQU     H'0006'

;----- STATUS Bits --------------------------------------------------------

PA2                          EQU     H'0007'
PA1                          EQU     H'0006'
PA0                          EQU     H'0005'
NOT_TO                       EQU     H'0004'
NOT_PD                       EQU     H'0003'
Z                            EQU     H'0002'
DC                           EQU     H'0001'
C                            EQU     H'0000'

;----- OPTION Bits --------------------------------------------------------

T0CS                         EQU     H'0005'
T0SE                         EQU     H'0004'
PSA                          EQU     H'0003'
PS2                          EQU     H'0002'
PS1                          EQU     H'0001'
PS0                          EQU     H'0000'

;==========================================================================
;
;       Processor-dependent Definitions
;
;==========================================================================

        IFDEF __16F54
           __MAXRAM H'01F'
        ENDIF

;--------------------------------------------------------------------------

        IFDEF __16F57
                                                ; Register Files
PORTC                     EQU     H'0007'
           __MAXRAM H'07F'
        ENDIF


;==========================================================================
;
;       Configuration Bits
;
;==========================================================================


_CP_ON                       EQU     H'0FF7'
_CP_OFF                      EQU     H'0FFF'
_WDT_ON                      EQU     H'0FFF'
_WDT_OFF                     EQU     H'0FFB'
_LP_OSC                      EQU     H'0FFC'
_XT_OSC                      EQU     H'0FFD'
_HS_OSC                      EQU     H'0FFE'
_RC_OSC                      EQU     H'0FFF'



 

	LIST
