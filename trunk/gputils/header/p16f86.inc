        LIST
; P16F86.INC  Standard Header File, Version 1.00    Microchip Technology, Inc.
        NOLIST

; This header file defines configurations, registers, and other useful bits of
; information for the PIC16F86 microcontroller.  These names are taken to match 
; the data sheets as closely as possible.  

; Note that the processor must be selected before this file is 
; included.  The processor may be selected the following ways:

;       1. Command line switch:
;               C:\ MPASM MYFILE.ASM /PIC16F86
;       2. LIST directive in the source file
;               LIST   P=PIC16F86
;       3. Processor Type entry in the MPASM full-screen interface

;==========================================================================
;
;       Revision History
;
;==========================================================================

;Rev:   Date:    Reason:

;1.00   03/22/01 Initial Release

;==========================================================================
;
;       Verify Processor
;
;==========================================================================

        IFNDEF __16F86
           MESSG "Processor-header file mismatch.  Verify selected processor."
        ENDIF

;==========================================================================
;
;       Register Definitions
;
;==========================================================================

W                            EQU     H'0000'
F                            EQU     H'0001'

;----- Register Files------------------------------------------------------

INDF                         EQU     H'0000'
TMR0                         EQU     H'0001'
PCL                          EQU     H'0002'
STATUS                       EQU     H'0003'
FSR                          EQU     H'0004'
PORTA                        EQU     H'0005'
PORTB                        EQU     H'0006'
PCLATH                       EQU     H'000A'
INTCON                       EQU     H'000B'
PIR1                         EQU     H'000C'
PIR2                         EQU     H'000D'
ADRES                        EQU     H'001E'
ADCON0                       EQU     H'001F'

OPTION_REG                   EQU     H'0081'
TRISA                        EQU     H'0085'
TRISB                        EQU     H'0086'
PIE1                         EQU     H'008C'
PIE2                         EQU     H'008D'
PCON                         EQU     H'008E'
CMCON                        EQU     H'009C'
CVRCON                       EQU     H'009D'
ADCON1                       EQU     H'009F'

EEDATA                       EQU     H'010C'
EEADR                        EQU     H'010D'
EEDATH                       EQU     H'010E'
EEADRH                       EQU     H'010F'

EECON1                       EQU     H'018C'
EECON2                       EQU     H'018D'

;----- STATUS Bits --------------------------------------------------------

IRP                          EQU     H'0007'
RP1                          EQU     H'0006'
RP0                          EQU     H'0005'
NOT_TO                       EQU     H'0004'
NOT_PD                       EQU     H'0003'
Z                            EQU     H'0002'
DC                           EQU     H'0001'
C                            EQU     H'0000'

;----- INTCON Bits --------------------------------------------------------

GIE                          EQU     H'0007'
PEIE                         EQU     H'0006'
TMR0IE                       EQU     H'0005'
INTE                         EQU     H'0004'
RBIE                         EQU     H'0003'
TMR0IF                       EQU     H'0002'
INTF                         EQU     H'0001'
RBIF                         EQU     H'0000'

;----- PIR1 Bits ----------------------------------------------------------

ADIF                         EQU     H'0006'

;----- PIR2 Bits ----------------------------------------------------------

CMIF			     EQU     H'0006'
EEIF                         EQU     H'0004'

;----- ADCON0 Bits --------------------------------------------------------

ADCS1                        EQU     H'0007'
ADCS0                        EQU     H'0006'
CHS2                         EQU     H'0005'
CHS1                         EQU     H'0004'
CHS0                         EQU     H'0003'
GO                           EQU     H'0002'
NOT_DONE                     EQU     H'0002'
GO_DONE                      EQU     H'0002'
ADON                         EQU     H'0000'

;----- OPTION_REG Bits -----------------------------------------------------

NOT_RBPU                     EQU     H'0007'
INTEDG                       EQU     H'0006'
T0CS                         EQU     H'0005'
T0SE                         EQU     H'0004'
PSA                          EQU     H'0003'
PS2                          EQU     H'0002'
PS1                          EQU     H'0001'
PS0                          EQU     H'0000'

;----- PIE1 Bits ----------------------------------------------------------

ADIE                         EQU     H'0006'

;----- PIE2 Bits ----------------------------------------------------------

CMIE			     EQU     H'0006'	
EEIE                         EQU     H'0004'

;----- PCON Bits ----------------------------------------------------------

NOT_POR                      EQU     H'0001'
NOT_BO                       EQU     H'0000'
NOT_BOR                      EQU     H'0000'

;----- CMCON Bits -----------------------------------------------------

C2OUT                        EQU     H'0007'
C1OUT                        EQU     H'0006'
C2INV                        EQU     H'0005'
C1INV                        EQU     H'0004'
CIS                          EQU     H'0003'
CM2                          EQU     H'0002'
CM1                          EQU     H'0001'
CM0                          EQU     H'0000'

;----- CVRCON Bits -----------------------------------------------------

CVREN                        EQU     H'0007'
CVROE                        EQU     H'0006'
CVRR                         EQU     H'0005'
CVR3                         EQU     H'0003'
CVR2                         EQU     H'0002'
CVR1                         EQU     H'0001'
CVR0                         EQU     H'0000'
  
;----- ADCON1 Bits --------------------------------------------------------

ADCS2                        EQU     H'0006'
PCFG2                        EQU     H'0002'
PCFG1                        EQU     H'0001'
PCFG0                        EQU     H'0000'

;----- EECON1 Bits --------------------------------------------------------

EEPGD                        EQU     H'0007'
WRERR                        EQU     H'0003'
WREN                         EQU     H'0002'
WR                           EQU     H'0001'
RD                           EQU     H'0000'

;==========================================================================
;
;       RAM Definition
;
;==========================================================================

        __MAXRAM H'1FF'
        __BADRAM H'07'- H'09', H'0E'- H'1D'
        __BADRAM H'87'- H'89', H'8F'- H'9B', H'9E', H'C0'- H'EF'
        __BADRAM H'105', H'107'- H'109', H'110'- H'11F'
	__BADRAM H'185', H'187'- H'189', H'190'- H'19F'

;==========================================================================
;
;       Configuration Bits
;
;==========================================================================

_CP_ALL                      EQU     H'0FFF'
_CP_OFF                      EQU     H'3FFF'
_DEBUG_ON                    EQU     H'37FF'
_DEBUG_OFF                   EQU     H'3FFF'
_WRT_OFF                     EQU     H'3FFF'
_WRT_256                     EQU     H'3DFF'
_WRT_HALF		     EQU     H'3BFF'
_WRT_3FOURTHS		     EQU     H'39FF'
_CPD_ON                      EQU     H'3EFF'
_CPD_OFF                     EQU     H'3FFF'
_LVP_ON                      EQU     H'3FFF'
_LVP_OFF                     EQU     H'3F7F'
_BODEN_ON                    EQU     H'3FFF'
_BODEN_OFF                   EQU     H'3FBF'
_MCLRE_ON                    EQU     H'3FFF'
_MCLRE_OFF                   EQU     H'3FDF'
_PWRTE_OFF                   EQU     H'3FFF'
_PWRTE_ON                    EQU     H'3FF7'
_WDT_ON                      EQU     H'3FFF'
_WDT_OFF                     EQU     H'3FFB'
_ER_OSC_CLKOUT               EQU     H'3FFF'
_ER_OSC_NOCLKOUT             EQU     H'3FFE'
_INTRC_OSC_CLKOUT            EQU     H'3FFD'
_INCRC_OSC_NOCLKOUT          EQU     H'3FFC'
_EXTCLK_OSC                  EQU     H'3FEF'
_LP_OSC                      EQU     H'3FEC'
_XT_OSC                      EQU     H'3FED'
_HS_OSC                      EQU     H'3FEE'

        LIST
