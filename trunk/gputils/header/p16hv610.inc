        LIST
; P16HV610.INC  Standard Header File, Version 1.00    Microchip Technology, Inc.
        NOLIST

; Based on P16F610.INC
; This header file defines configurations, registers, and other useful bits of
; information for the PIC16HV610 microcontroller.  These names are taken to 
; match the data sheets as closely as possible.  

; Note that the processor must be selected before this file is 
; included.  The processor may be selected the following ways:

;       1. Command line switch:
;               C:\ MPASM MYFILE.ASM /PIC16HV610
;       2. LIST directive in the source file
;               LIST   P=PIC16HV610
;       3. Processor Type entry in the MPASM full-screen interface

;==========================================================================
;
;       Revision History
;
;==========================================================================
;1.00   07/10/06 Original
;1.01   12/04/06 Added WPU, BOR and IOC aliases
;1.02	01/15/07 Added TRISA & TRISC bits
;==========================================================================
;
;       Verify Processor
;
;==========================================================================

        IFNDEF __16HV610
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

PORTC                        EQU     H'0007'

PCLATH                       EQU     H'000A'
INTCON                       EQU     H'000B'
PIR1                         EQU     H'000C'

TMR1L                        EQU     H'000E'
TMR1H                        EQU     H'000F'
T1CON                        EQU     H'0010'

VRCON                        EQU     H'0019'
CM1CON0                      EQU     H'001A'
CM2CON0                      EQU     H'001B'
CM2CON1                      EQU     H'001C'

OPTION_REG                   EQU     H'0081'

TRISA                        EQU     H'0085'

TRISC                        EQU     H'0087'

PIE1                         EQU     H'008C'

PCON                         EQU     H'008E'

OSCTUNE                      EQU     H'0090'
ANSEL                        EQU     H'0091'

WPUA                         EQU     H'0095'
WPU                          EQU     H'0095'
IOCA                         EQU     H'0096'
IOC                          EQU     H'0096'

SRCON0                       EQU     H'0099'
SRCON1                       EQU     H'009A'


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
T0IE                         EQU     H'0005'
INTE                         EQU     H'0004'
RAIE                         EQU     H'0003'
T0IF                         EQU     H'0002'
INTF                         EQU     H'0001'
RAIF                         EQU     H'0000'

;----- PORTA Bits ---------------------------------------------------------

RA5                          EQU     H'0005'
RA4                          EQU     H'0004'
RA3                          EQU     H'0003'
RA2                          EQU     H'0002'
RA1                          EQU     H'0001'
RA0                          EQU     H'0000'

;----- PORTC Bits ---------------------------------------------------------

RC5                          EQU     H'0005'
RC4                          EQU     H'0004'
RC3                          EQU     H'0003'
RC2                          EQU     H'0002'
RC1                          EQU     H'0001'
RC0                          EQU     H'0000'

;----- PIR1 Bits ----------------------------------------------------------

C2IF                         EQU     H'0004'
C1IF                         EQU     H'0003'

TMR1IF                       EQU     H'0000'
T1IF                         EQU     H'0000'

;----- T1CON Bits ---------------------------------------------------------

T1GINV                       EQU     H'0007'
TMR1GE                       EQU     H'0006'
T1CKPS1                      EQU     H'0005'
T1CKPS0                      EQU     H'0004'
T1OSCEN                      EQU     H'0003'
NOT_T1SYNC                   EQU     H'0002'
TMR1CS                       EQU     H'0001'
TMR1ON                       EQU     H'0000'

;----- VRCON Bits ---------------------------------------------------------

C1VREN                       EQU     H'0007'
C2VREN                       EQU     H'0006'
VRR                          EQU     H'0005'
FVREN                        EQU     H'0004'
VP6EN                        EQU     H'0004'
VR3                          EQU     H'0003'
VR2                          EQU     H'0002'
VR1                          EQU     H'0001'
VR0                          EQU     H'0000'

;----- CM1CON0 Bits -------------------------------------------------------

C1ON                         EQU     H'0007'
C1OUT                        EQU     H'0006'
C1OE                         EQU     H'0005'
C1POL                        EQU     H'0004'

C1R                          EQU     H'0002'
C1CH1                        EQU     H'0001'
C1CH0                        EQU     H'0000'

;----- CM2CON0 Bits -------------------------------------------------------

C2ON                         EQU     H'0007'
C2OUT                        EQU     H'0006'
C2OE                         EQU     H'0005'
C2POL                        EQU     H'0004'

C2R                          EQU     H'0002'
C2CH1                        EQU     H'0001'
C2CH0                        EQU     H'0000'

;----- CM2CON1 Bits -------------------------------------------------------

MC1OUT                       EQU      H'0007'
MC2OUT                       EQU      H'0006'

T1ACS                        EQU      H'0004'
C1HYS                        EQU      H'0003'
C2HYS                        EQU      H'0002'
T1GSS                        EQU      H'0001'
C2SYNC                       EQU      H'0000'

;----- OPTION Bits --------------------------------------------------------

NOT_RAPU            	     EQU     H'0007'
INTEDG                       EQU     H'0006'
T0CS                         EQU     H'0005'
T0SE                         EQU     H'0004'
PSA                          EQU     H'0003'
PS2                          EQU     H'0002'
PS1                          EQU     H'0001'
PS0                          EQU     H'0000'

;----- TRISA Bits --------------------------------------------------------

TRISA5                      EQU     H'0005'
TRISA4                      EQU     H'0004'
TRISA3                      EQU     H'0003'
TRISA2                      EQU     H'0002'
TRISA1                      EQU     H'0001'
TRISA0                      EQU     H'0000'

;----- TRISC Bits --------------------------------------------------------

TRISC5                      EQU     H'0005'
TRISC4                      EQU     H'0004'
TRISC3                      EQU     H'0003'
TRISC2                      EQU     H'0002'
TRISC1                      EQU     H'0001'
TRISC0                      EQU     H'0000'

;----- PIE1 Bits ----------------------------------------------------------

C2IE                         EQU     H'0004'
C1IE                         EQU     H'0003'

TMR1IE                       EQU     H'0000'
T1IE                         EQU     H'0000'

;----- PCON Bits ----------------------------------------------------------

NOT_POR                      EQU     H'0001'
NOT_BOD                      EQU     H'0000'
NOT_BOR                      EQU     H'0000'

;----- OSCTUNE Bits -------------------------------------------------------

TUN4                         EQU     H'0004'
TUN3                         EQU     H'0003'
TUN2                         EQU     H'0002'
TUN1                         EQU     H'0001'
TUN0                         EQU     H'0000'

;----- ANSEL Bits ---------------------------------------------------------

ANS7                         EQU     H'0007'
ANS6                         EQU     H'0006'
ANS5                         EQU     H'0005'
ANS4                         EQU     H'0004'

ANS1                         EQU     H'0001'
ANS0                         EQU     H'0000'

;----- WPUA Bits ---------------------------------------------------------

WPUA5                        EQU     H'0005'
WPUA4                        EQU     H'0004'

WPUA2                        EQU     H'0002'
WPUA1                        EQU     H'0001'
WPUA0                        EQU     H'0000'

;----- WPU Bits ---------------------------------------------------------

WPU5                         EQU     H'0005'
WPU4                         EQU     H'0004'

WPU2                         EQU     H'0002'
WPU1                         EQU     H'0001'
WPU0                         EQU     H'0000'

;----- IOCA Bits ---------------------------------------------------------

IOCA5                        EQU     H'0005'
IOCA4                        EQU     H'0004'
IOCA3                        EQU     H'0003'
IOCA2                        EQU     H'0002'
IOCA1                        EQU     H'0001'
IOCA0                        EQU     H'0000'

;----- IOC Bits ---------------------------------------------------------

IOC5                         EQU     H'0005'
IOC4                         EQU     H'0004'
IOC3                         EQU     H'0003'
IOC2                         EQU     H'0002'
IOC1                         EQU     H'0001'
IOC0                         EQU     H'0000'

;----- SRCON0 Bits -------------------------------------------------------

SR1                          EQU     H'0007'
SR0                          EQU     H'0006'
C1SEN                        EQU     H'0005'
C2REN                        EQU     H'0004'
PULSS                        EQU     H'0003'
PULSR                        EQU     H'0002'

SRCLKEN                      EQU     H'0000'

;----- SRCON1 Bits -------------------------------------------------------

SRCS1                        EQU     H'0007'
SRCS0                        EQU     H'0006'

;==========================================================================
;
;       RAM Definition
;
;==========================================================================

        __MAXRAM H'FF'
        __BADRAM H'06', H'08'-H'09', H'0D', H'11' -H'18', H'1D'-H'1F', H'20' -H'3F'
        __BADRAM H'86', H'88'-H'89', H'8D', H'8F', H'92'-H'94', H'97'-H'98', H'9B' -H'9F', H'A0'-H'EF'

;==========================================================================
;
;       Configuration Bits
;
;==========================================================================
		

_BOD_ON                      EQU     H'3FFF'
_BOR_ON                      EQU     H'3FFF'
_BOD_NSLEEP                  EQU     H'3EFF'
_BOR_NSLEEP                  EQU     H'3EFF'
_BOD_OFF                     EQU     H'3CFF'
_BOR_OFF                     EQU     H'3CFF'
_IOSCFS8                     EQU     H'3FFF'
_IOSCFS_8MHZ                 EQU     H'3FFF'
_IOSCFS4                     EQU     H'3F7F'
_IOSCFS_4MHZ                 EQU     H'3F7F'
_CP_OFF                      EQU     H'3FFF'
_CP_ON                       EQU     H'3FBF'
_MCLRE_ON                    EQU     H'3FFF'
_MCLRE_OFF                   EQU     H'3FDF'
_PWRTE_OFF                   EQU     H'3FFF'
_PWRTE_ON                    EQU     H'3FEF'
_WDT_ON                      EQU     H'3FFF'
_WDT_OFF                     EQU     H'3FF7'
_LP_OSC                      EQU     H'3FF8'
_XT_OSC                      EQU     H'3FF9'
_HS_OSC                      EQU     H'3FFA'
_EC_OSC                      EQU     H'3FFB'
_INTRC_OSC_NOCLKOUT          EQU     H'3FFC'
_INTRC_OSC_CLKOUT            EQU     H'3FFD'
_EXTRC_OSC_NOCLKOUT          EQU     H'3FFE'
_EXTRC_OSC_CLKOUT            EQU     H'3FFF'
_INTOSCIO                    EQU     H'3FFC'
_INTOSC                      EQU     H'3FFD'
_EXTRCIO                     EQU     H'3FFE'
_EXTRC                       EQU     H'3FFF'

        LIST
