        LIST
; PMCV14A.INC  Standard Header File, Version 1.00    Microchip Technology, Inc.
        NOLIST



;       1. Command line switch:
;               C:\ MPASM MYFILE.ASM /PMCV14A
;       2. LIST directive in the source file
;               LIST   P=MCV14A
;       3. Processor Type entry in the MPASM full-screen interface

;==========================================================================
;
;          Revision History
;
;==========================================================================

;Rev:   Date:    Reason:

;1.00   1/3/2008  New Revision.
;
;
;==========================================================================
;
;       Verify Processor
;
;==========================================================================

        IFNDEF __MCV14A
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
PORTC                        EQU     H'0007'
CM1CON0                      EQU     H'0008'
ADCON0                       EQU     H'0009'
ADRES                        EQU     H'000A'
CM2CON0                      EQU     H'000B'
VRCON                        EQU     H'000C'
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
CWUF                         EQU     H'0006'
PA0                          EQU     H'0005'
NOT_TO                       EQU     H'0004'
NOT_PD                       EQU     H'0003'
Z                            EQU     H'0002'
DC                           EQU     H'0001'
C                            EQU     H'0000'

;----- VRCON0 Bits ---------------------------------------------------------

VREN                         EQU     H'0007'
VROE                         EQU     H'0006'
VRR                          EQU     H'0005'
VR3                          EQU     H'0003'
VR2                          EQU     H'0002'
VR1                          EQU     H'0001'
VR0                          EQU     H'0000'

;----- CM1CON0 Bits -------------------------------------------------------

C1OUT                        EQU     H'0007'
NOT_C1OUTEN                  EQU     H'0006'
C1POL                        EQU     H'0005'
NOT_C1T0CS                   EQU     H'0004'
C1ON                         EQU     H'0003'
C1NREF                       EQU     H'0002'
C1PREF                       EQU     H'0001'
NOT_C1WU                     EQU     H'0000'

;----- CM2CON0 Bits -------------------------------------------------------

C2OUT                        EQU     H'0007'
NOT_C2OUTEN                  EQU     H'0006'
C2POL                        EQU     H'0005'
C2PREF2                      EQU     H'0004'
C2ON                         EQU     H'0003'
C2NREF                       EQU     H'0002'
C2PREF1                      EQU     H'0001'
NOT_C2WU                     EQU     H'0000'

;----- ADCON0 Bits --------------------------------------------------------

ANS1                         EQU     H'0007'
ANS0                         EQU     H'0006'
ADCS1                        EQU     H'0005'
ADCS0                        EQU     H'0004'
CHS1                         EQU     H'0003'
CHS0                         EQU     H'0002'
GO                           EQU     H'0001'
NOT_DONE                     EQU     H'0001'
ADON                         EQU     H'0000'

;----- ADRES Bits --------------------------------------------------------

ADRES7                       EQU     H'0007'
ADRES6                       EQU     H'0006'
ADRES5                       EQU     H'0005'
ADRES4                       EQU     H'0004'
ADRES3                       EQU     H'0003'
ADRES2                       EQU     H'0002'
ADRES1                       EQU     H'0001'
ADRES0                       EQU     H'0000'


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

        __MAXRAM H'7F'

;==========================================================================
;
;       Configuration Bits
;
;==========================================================================

_CPDF_OFF                    EQU     H'0FFF'
_CPDF_ON                     EQU     H'0F7F'
_IOSCFS_8MHz                 EQU     H'0FFF'
_IOSCFS_4MHz                 EQU     H'0FBF'
_MCLRE_ON                    EQU     H'0FFF'
_MCLRE_OFF                   EQU     H'0FDF'
_CP_ON                       EQU     H'0FEF'
_CP_OFF                      EQU     H'0FFF'
_WDTE_ON                     EQU     H'0FFF'
_WDTE_OFF                    EQU     H'0FF7'
_LP_OSC                      EQU     H'0FF8'
_XT_OSC                      EQU     H'0FF9'
_HS_OSC                      EQU     H'0FFA'
_EC_OSC                      EQU     H'0FFB'
_IntRC_OSC_RB4               EQU     H'0FFC'
_IntRC_OSC_CLKOUT            EQU     H'0FFD'
_ExtRC_OSC_RB4               EQU     H'0FFE'
_ExtRC_OSC_CLKOUT            EQU     H'0FFF'

        LIST
