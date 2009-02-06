        LIST
; P16LF723.INC  Standard Header File, Version 2.01    Microchip Technology, Inc.
        NOLIST

; This header file defines configurations, registers, and other useful bits of
; information for the PIC16LF723 microcontroller.
; These names are taken to match the data sheets as closely as possible.

; Note that the processor must be selected before this file is
; included.  The processor may be selected the following ways:

;       1. Command line switch:
;               C:\ MPASM MYFILE.ASM /PIC16LF723
;       2. LIST directive in the source file
;               LIST   P=PIC16LF723
;       3. Processor Type entry in the MPASM full-screen interface

;==========================================================================
;
;       Revision History
;
;==========================================================================

;Rev:   Date:    Reason:
;0.01	10/25/07 Initial template
;1.00	11/06/07 Version 1.00 (no changes)
;1.01	11/27/07 Remove 108h-109h (CPSCON0 and CPSCON1) from BADRAM
;2.00	12/18/07 Flatten include files not to include one master file
;2.01	01/31/08 Correct swapped definitions of VCAP_RA5 and VCAP_RA6


;==========================================================================
;
;       Verify Processor
;
;==========================================================================

        IFNDEF __16LF723
           MESSG "Processor-header file mismatch.  Verify selected processor."
#define	   __16LF723
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
PORTC                        EQU     H'0007'
PORTE                        EQU     H'0009'
PCLATH                       EQU     H'000A'
INTCON                       EQU     H'000B'
PIR1                         EQU     H'000C'
PIR2                         EQU     H'000D'
TMR1L                        EQU     H'000E'
TMR1H                        EQU     H'000F'
T1CON                        EQU     H'0010'
TMR2                         EQU     H'0011'
T2CON                        EQU     H'0012'
SSPBUF                       EQU     H'0013'
SSPCON                       EQU     H'0014'
CCPR1L                       EQU     H'0015'
CCPR1H                       EQU     H'0016'
CCP1CON                      EQU     H'0017'
RCSTA                        EQU     H'0018'
TXREG                        EQU     H'0019'
RCREG                        EQU     H'001A'
CCPR2L                       EQU     H'001B'
CCPR2H                       EQU     H'001C'
CCP2CON                      EQU     H'001D'
ADRES                        EQU     H'001E'
ADCON0                       EQU     H'001F'

OPTION_REG                   EQU     H'0081'
TRISA                        EQU     H'0085'
TRISB                        EQU     H'0086'
TRISC                        EQU     H'0087'
TRISE                        EQU     H'0089'
PIE1                         EQU     H'008C'
PIE2                         EQU     H'008D'
PCON                         EQU     H'008E'
T1GCON			     EQU     H'008F'
OSCCON			     EQU     H'0090'
OSCTUNE			     EQU     H'0091'
PR2                          EQU     H'0092'
SSPADD                       EQU     H'0093'
SSPMSK                       EQU     H'0093'
SSPSTAT                      EQU     H'0094'
WPUB			     EQU     H'0095'
IOCB			     EQU     H'0096'
IOC			     EQU     H'0096'
TXSTA                        EQU     H'0098'
SPBRG                        EQU     H'0099'
APFCON                       EQU     H'009C'
FVRCON                       EQU     H'009D'
ADCON1                       EQU     H'009F'

CPSCON0			     EQU     H'0108'
CPSCON1			     EQU     H'0109'
PMDATL	                     EQU     H'010C'
PMDATA                       EQU     H'010C'    ; Backward compatibility only
PMADRL                       EQU     H'010D'
PMADR                        EQU     H'010D'    ; Backward compatibility only
PMDATH                       EQU     H'010E'
PMADRH                       EQU     H'010F'


ANSELA			     EQU     H'0185'
ANSELB			     EQU     H'0186'
PMCON1                       EQU     H'018C'

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
TMR0IE                       EQU     H'0005'
INTE                         EQU     H'0004'
RBIE                         EQU     H'0003'
IOCIE                        EQU     H'0003'	;; USE IOCIE instead of RBIE to be generic
T0IF                         EQU     H'0002'
TMR0IF                       EQU     H'0002'
INTF                         EQU     H'0001'
RBIF                         EQU     H'0000'
IOCIF                        EQU     H'0000'	;; USE IOCIF instead of RBIF to be generic

;----- PIR1 Bits ----------------------------------------------------------

TMR1GIF                      EQU     H'0007'
ADIF                         EQU     H'0006'
RCIF                         EQU     H'0005'
TXIF                         EQU     H'0004'
SSPIF                        EQU     H'0003'
CCP1IF                       EQU     H'0002'
TMR2IF                       EQU     H'0001'
TMR1IF                       EQU     H'0000'

;----- PIR2 Bits ----------------------------------------------------------

CCP2IF                       EQU     H'0000'

;----- T1CON Bits ---------------------------------------------------------

TMR1CS1                      EQU     H'0007'
TMR1CS0                      EQU     H'0006'
T1CKPS1                      EQU     H'0005'
T1CKPS0                      EQU     H'0004'
T1OSCEN                      EQU     H'0003'
NOT_T1SYNC                   EQU     H'0002'
T1INSYNC                     EQU     H'0002'    ; Backward compatibility only
T1SYNC                       EQU     H'0002'
TMR1ON                       EQU     H'0000'

;----- T2CON Bits ---------------------------------------------------------

TOUTPS3                      EQU     H'0006'
TOUTPS2                      EQU     H'0005'
TOUTPS1                      EQU     H'0004'
TOUTPS0                      EQU     H'0003'
TMR2ON                       EQU     H'0002'
T2CKPS1                      EQU     H'0001'
T2CKPS0                      EQU     H'0000'

;----- SSPCON Bits --------------------------------------------------------

WCOL                         EQU     H'0007'
SSPOV                        EQU     H'0006'
SSPEN                        EQU     H'0005'
CKP                          EQU     H'0004'
SSPM3                        EQU     H'0003'
SSPM2                        EQU     H'0002'
SSPM1                        EQU     H'0001'
SSPM0                        EQU     H'0000'

;----- CCP1CON Bits -------------------------------------------------------

DC1B1                        EQU     H'0005'
DC1B0                        EQU     H'0004'
CCP1X                        EQU     H'0005'    ; Backward compatibility only
CCP1Y                        EQU     H'0004'    ; Backward compatibility only
CCP1M3                       EQU     H'0003'
CCP1M2                       EQU     H'0002'
CCP1M1                       EQU     H'0001'
CCP1M0                       EQU     H'0000'

;----- RCSTA Bits ---------------------------------------------------------

SPEN                         EQU     H'0007'
RX9                          EQU     H'0006'
RC9                          EQU     H'0006'    ; Backward compatibility only
NOT_RC8                      EQU     H'0006'    ; Backward compatibility only
RC8_9                        EQU     H'0006'    ; Backward compatibility only
SREN                         EQU     H'0005'
CREN                         EQU     H'0004'
ADDEN                        EQU     H'0003'
FERR                         EQU     H'0002'
OERR                         EQU     H'0001'
RX9D                         EQU     H'0000'
RCD8                         EQU     H'0000'    ; Backward compatibility only

;----- CCP2CON Bits -------------------------------------------------------
DC2B1                        EQU     H'0005'
DC2B0                        EQU     H'0004'
CCP2X                        EQU     H'0005'    ; Backward compatibility only
CCP2Y                        EQU     H'0004'    ; Backward compatibility only
CCP2M3                       EQU     H'0003'
CCP2M2                       EQU     H'0002'
CCP2M1                       EQU     H'0001'
CCP2M0                       EQU     H'0000'

;----- ADCON0 Bits --------------------------------------------------------

CHS3                         EQU     H'0005'
CHS2                         EQU     H'0004'
CHS1                         EQU     H'0003'
CHS0                         EQU     H'0002'
GO                           EQU     H'0001'
NOT_DONE                     EQU     H'0001'
GO_DONE                      EQU     H'0001'
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

TMR1GIE                      EQU     H'0007'
ADIE                         EQU     H'0006'
RCIE                         EQU     H'0005'
TXIE                         EQU     H'0004'
SSPIE                        EQU     H'0003'
CCP1IE                       EQU     H'0002'
TMR2IE                       EQU     H'0001'
TMR1IE                       EQU     H'0000'

;----- PIE2 Bits ----------------------------------------------------------

CCP2IE                       EQU     H'0000'

;----- PCON Bits ----------------------------------------------------------

NOT_POR                      EQU     H'0001'
NOT_BO                       EQU     H'0000' ;; Backwards Compatability only
NOT_BOR                      EQU     H'0000'


;----- T1GCON Bits ----------------------------------------------------------

TMR1GE			     EQU     H'0007'
T1GPOL			     EQU     H'0006'
T1GTM			     EQU     H'0005'
T1GSPM			     EQU     H'0004'
T1G_NOT_DONE                 EQU     H'0003'
T1GGO_DONE                   EQU     H'0003'
T1GGO                        EQU     H'0003'
T1GVAL			     EQU     H'0002'
T1GSS1			     EQU     H'0001'
T1GSS0			     EQU     H'0000'

;----- OSCCON Bits -------------------------------------------------------

IRCF1			     EQU     H'0005'
IRCF0			     EQU     H'0004'
ICSL			     EQU     H'0003'
ICSS                         EQU     H'0002'

;----- OSCTUNE Bits -------------------------------------------------------

TUN5                         EQU     H'0005'
TUN4                         EQU     H'0004'
TUN3                         EQU     H'0003'
TUN2		             EQU     H'0002'
TUN1                         EQU     H'0001'
TUN0		             EQU     H'0000'


;----- SSPSTAT Bits -------------------------------------------------------

SMP                          EQU     H'0007'
CKE                          EQU     H'0006'
D                            EQU     H'0005'
I2C_DATA                     EQU     H'0005'
NOT_A                        EQU     H'0005'
NOT_ADDRESS                  EQU     H'0005'
D_A                          EQU     H'0005'
DATA_ADDRESS                 EQU     H'0005'
P                            EQU     H'0004'
I2C_STOP                     EQU     H'0004'
S                            EQU     H'0003'
I2C_START                    EQU     H'0003'
R                            EQU     H'0002'
I2C_READ                     EQU     H'0002'
NOT_W                        EQU     H'0002'
NOT_WRITE                    EQU     H'0002'
R_W                          EQU     H'0002'
READ_WRITE                   EQU     H'0002'
UA                           EQU     H'0001'
BF                           EQU     H'0000'


;----- WPUB Bits -------------------------------------------------------

WPUB7                        EQU     H'0007'
WPUB6                        EQU     H'0006'
WPUB5                        EQU     H'0005'
WPUB4                        EQU     H'0004'
WPUB3                        EQU     H'0003'
WPUB2                        EQU     H'0002'
WPUB1                        EQU     H'0001'
WPUB0                        EQU     H'0000'

;----- WPU Bits -------------------------------------------------------

WPU7                         EQU     H'0007'	; Backward Compatability only
WPU6                         EQU     H'0006'	; Backward Compatability only
WPU5                         EQU     H'0005'	; Backward Compatability only
WPU4                         EQU     H'0004'	; Backward Compatability only
WPU3                         EQU     H'0003'	; Backward Compatability only
WPU2                         EQU     H'0002'	; Backward Compatability only
WPU1                         EQU     H'0001'	; Backward Compatability only
WPU0                         EQU     H'0000'	; Backward Compatability only

;----- IOCB Bits -------------------------------------------------------

IOCB7                        EQU     H'0007'
IOCB6                        EQU     H'0006'
IOCB5                        EQU     H'0005'
IOCB4                        EQU     H'0004'
IOCB3                        EQU     H'0003'
IOCB2                        EQU     H'0002'
IOCB1                        EQU     H'0001'
IOCB0                        EQU     H'0000'


;----- IOC Bits -------------------------------------------------------

IOC7                         EQU     H'0007'	; Backward Compatability only
IOC6                         EQU     H'0006'	; Backward Compatability only
IOC5                         EQU     H'0005'	; Backward Compatability only
IOC4                         EQU     H'0004'	; Backward Compatability only
IOC3                         EQU     H'0003'	; Backward Compatability only
IOC2                         EQU     H'0002'	; Backward Compatability only
IOC1                         EQU     H'0001'	; Backward Compatability only
IOC0                         EQU     H'0000'	; Backward Compatability only


;----- TXSTA Bits ---------------------------------------------------------

CSRC                         EQU     H'0007'
TX9                          EQU     H'0006'
NOT_TX8                      EQU     H'0006'    ; Backward compatibility only
TX8_9                        EQU     H'0006'    ; Backward compatibility only
TXEN                         EQU     H'0005'
SYNC                         EQU     H'0004'
BRGH                         EQU     H'0002'
TRMT                         EQU     H'0001'
TX9D                         EQU     H'0000'
TXD8                         EQU     H'0000'    ; Backward compatibility only


;----- APFCON Bits ---------------------------------------------------------

CCP2SEL                      EQU     H'0000'
SSSEL                        EQU     H'0001'


;----- FVRCON Bits --------------------------------------------------------

FVRST                        EQU     H'0007'
FVREN                        EQU     H'0006'
ADFVR1                       EQU     H'0001'
ADFVR0                       EQU     H'0000'

;----- ADCON1 Bits --------------------------------------------------------

ADCS2			     EQU     H'0006'
ADCS1			     EQU     H'0005'
ADCS0			     EQU     H'0004'
ADREF1			     EQU     H'0001'
ADREF0			     EQU     H'0000'



;----- CPSCON0 Bits ---------------------------------------------------------

CPSON			     EQU     H'0007'
CPSRNG1			     EQU     H'0003'
CPSRNG0			     EQU     H'0002'
CPSOUT			     EQU     H'0001'
T0XCS			     EQU     H'0000'

;----- CPSCON1 Bits ---------------------------------------------------------

CPSCH3			     EQU     H'0003'
CPSCH2			     EQU     H'0002'
CPSCH1			     EQU     H'0001'
CPSCH0			     EQU     H'0000'

;----- ANSELA Bits ---------------------------------------------------------

ANSA5			     EQU     H'0005'
ANSA4			     EQU     H'0004'
ANSA3			     EQU     H'0003'
ANSA2			     EQU     H'0002'
ANSA1			     EQU     H'0001'
ANSA0			     EQU     H'0000'

;----- ANSELB Bits ---------------------------------------------------------

ANSB5			     EQU     H'0005'
ANSB4			     EQU     H'0004'
ANSB3			     EQU     H'0003'
ANSB2			     EQU     H'0002'
ANSB1			     EQU     H'0001'
ANSB0			     EQU     H'0000'

;----- PMCON1 Bits --------------------------------------------------------

RD			     EQU     H'0000'
PMRD			     EQU     H'0000'



;==========================================================================
;
;       RAM Definition
;
;==========================================================================

        __MAXRAM H'1FF'

;==========================================================================
;
;       Configuration Bits
;
;==========================================================================

_CONFIG1                     EQU     H'2007'
_CONFIG2                     EQU     H'2008'

;----- Configuration Word 1 Bits ------------------------------------------

_DEBUG_ON		     EQU     H'1FFF'
_DEBUG_OFF                   EQU     H'3FFF'

_PLL_EN                      EQU     H'3FFF'
_PLL_DIS                     EQU     H'2FFF'

_BORV_1_9                    EQU     H'3FFF'
_BORV_2_5                    EQU     H'3BFF'

_BOR_ON                      EQU     H'3FFF'
_BOR_NSLEEP                  EQU     H'3DFF'
_BOR_OFF                     EQU     H'3CFF'

_CP_ON                       EQU     H'3FBF'
_CP_OFF                      EQU     H'3FFF'

_MCLR_EN                     EQU     H'3FFF'
_MCLRE_ON                    EQU     H'3FFF'	 ; Backward compatibility only
_MCLR_DIS                    EQU     H'3FDF'
_MCLRE_OFF                   EQU     H'3FDF'	 ; Backward compatibility only

_PWRT_EN                     EQU     H'3FEF'
_PWRTE_ON                    EQU     H'3FEF'	 ; Backward compatibility only
_PWRT_DIS                    EQU     H'3FFF'
_PWRTE_OFF                   EQU     H'3FFF'	 ; Backward compatibility only

_WDT_ON                      EQU     H'3FFF'
_WDT_OFF                     EQU     H'3FF7'

_LP_OSC                      EQU     H'3FF8'
_XT_OSC                      EQU     H'3FF9'
_HS_OSC                      EQU     H'3FFA'
_EC_OSC                      EQU     H'3FFB'
_INTRC_OSC_NOCLKOUT          EQU     H'3FFC'
_INTRC_OSC_CLKOUT            EQU     H'3FFD'
_INTOSCIO                    EQU     H'3FFC'
_INTOSC                      EQU     H'3FFD'
_EXTRC_OSC_NOCLKOUT          EQU     H'3FFE'
_EXTRC_OSC_CLKOUT            EQU     H'3FFF'
_EXTRCIO                     EQU     H'3FFE'
_EXTRC                       EQU     H'3FFF'


;----- Configuration Word 2 Bits ------------------------------------------

_VCAP_DIS                    EQU     H'3FFF'
_VCAP_RA6                    EQU     H'3FEF'
_VCAP_RA5                    EQU     H'3FDF'
_VCAP_RA0                    EQU     H'3FCF'

;==========================================================================
;
;       RAM Definition
;
;==========================================================================

	__BADRAM H'08'
	__BADRAM H'88'
        __BADRAM H'97', H'9A'-H'9B', H'9E'
        __BADRAM H'105'-H'107'
	__BADRAM H'110'-H'11F', H'130'-H'16F'
        __BADRAM H'187'-H'189', H'18D'-H'18F'
	__BADRAM H'190'-H'1EF'


        LIST
