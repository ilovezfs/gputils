        LIST
; P12F519.INC  Standard Header File, Version 1.00    Microchip Technology, Inc.
        NOLIST

; This header file defines configurations, registers, and other useful bits of
; information for the PIC12F509 microcontroller.  These names are taken to match 
; the data sheets as closely as possible.  

; Note that the processor must be selected before this file is 
; included.  The processor may be selected the following ways:

;       1. Command line switch:
;               C:\ MPASM MYFILE.ASM /P12F519
;       2. LIST directive in the source file
;               LIST   P=12F519
;       3. Processor Type entry in the MPASM full-screen interface

;==========================================================================
;
;          Revision History
;
;==========================================================================

;Rev:   Date:    Reason:

;1.00   1/24/07 Initial Release


;==========================================================================
;
;       Verify Processor
;
;==========================================================================

        IFNDEF __12F519
           MESSG "Processor-header file mismatch.  Verify selected processor."
        ENDIF


;==========================================================================
;
;       Register Definitions
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
OSCCAL                       EQU     H'0005'
PORTB                        EQU     H'0006'
EECON                        EQU     H'0021'
EEDATA                       EQU     H'0025'
EEADR                        EQU     H'0026'

;----- EECON -------------------------------------------------------------
FREE                         EQU     H'0004'
WRERR                        EQU     H'0003'
WREN                         EQU     H'0002'
WR                           EQU     H'0001'
RD                           EQU     H'0000'

;----- STATUS Bits --------------------------------------------------------

RBWUF                        EQU     H'0007'
PA0                          EQU     H'0005'
NOT_TO                       EQU     H'0004'
NOT_PD                       EQU     H'0003'
Z                            EQU     H'0002'
DC                           EQU     H'0001'
C                            EQU     H'0000'

;----- OPTION Bits --------------------------------------------------------

NOT_RBWU                     EQU     H'0007'
NOT_RBPU                     EQU     H'0006'
T0CS                         EQU     H'0005'
T0SE                         EQU     H'0004'
PSA                          EQU     H'0003'
PS2                          EQU     H'0002'
PS1                          EQU     H'0001'
PS0                          EQU     H'0000'

;----- OSCCAL Bits --------------------------------------------------------

CAL6                         EQU     H'0007'
CAL5                         EQU     H'0006'
CAL4                         EQU     H'0005'
CAL3                         EQU     H'0004'
CAL2                         EQU     H'0003'
CAL1                         EQU     H'0002'
CAL0                         EQU     H'0001'

;==========================================================================
;
;       RAM Definition
;
;==========================================================================

        __MAXRAM H'3F'

;==========================================================================
;
;       Configuration Bits
;
;==========================================================================

_CPDF_OFF                    EQU     H'0FFF'
_CPDF_ON                     EQU     H'0FBF'
_IOSCFS_8MHz                 EQU     H'0FFF'
_IOSCFS_4MHz                 EQU     H'0FDF'
_MCLRE_ON                    EQU     H'0FFF'
_MCLRE_OFF                   EQU     H'0FEF'
_CP_ON                       EQU     H'0FF7'
_CP_OFF                      EQU     H'0FFF'
_WDTE_ON                     EQU     H'0FFF'
_WDTE_OFF                    EQU     H'0FFB'
_LP_OSC                      EQU     H'0FFC'
_XT_OSC                      EQU     H'0FFD'
_IntRC_OSC                   EQU     H'0FFE'
_ExtRC_OSC                   EQU     H'0FFF'

        LIST
