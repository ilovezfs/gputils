        LIST

;==========================================================================
; Build date : May 15 2016
;  MPASM PIC16F15354 processor include
; 
;  (c) Copyright 1999-2016 Microchip Technology, All rights reserved
;==========================================================================

        NOLIST

;==========================================================================
;  This header file defines configurations, registers, and other useful
;  bits of information for the PIC16F15354 microcontroller.  These names
;  are taken to match the data sheets as closely as possible.
;
;  Note that the processor must be selected before this file is included.
;  The processor may be selected the following ways:
;
;       1. Command line switch:
;               C:\MPASM MYFILE.ASM /PIC16F15354
;       2. LIST directive in the source file
;               LIST   P=PIC16F15354
;       3. Processor Type entry in the MPASM full-screen interface
;       4. Setting the processor in the MPLAB Project Dialog
;==========================================================================

;==========================================================================
;
;       Verify Processor
;
;==========================================================================
        IFNDEF __16F15354
           MESSG "Processor-header file mismatch.  Verify selected processor."
        ENDIF



;==========================================================================
;
;       Register Definitions
;
;==========================================================================

W                EQU  H'0000'
F                EQU  H'0001'

;----- Register Files -----------------------------------------------------

;-----Bank0------------------
INDF0            EQU  H'0000'
INDF1            EQU  H'0001'
PCL              EQU  H'0002'
STATUS           EQU  H'0003'
FSR0             EQU  H'0004'
FSR0L            EQU  H'0004'
FSR0H            EQU  H'0005'
FSR1             EQU  H'0006'
FSR1L            EQU  H'0006'
FSR1H            EQU  H'0007'
BSR              EQU  H'0008'
WREG             EQU  H'0009'
PCLATH           EQU  H'000A'
INTCON           EQU  H'000B'
PORTA            EQU  H'000C'
PORTB            EQU  H'000D'
PORTC            EQU  H'000E'
PORTE            EQU  H'0010'
TRISA            EQU  H'0012'
TRISB            EQU  H'0013'
TRISC            EQU  H'0014'
LATA             EQU  H'0018'
LATB             EQU  H'0019'
LATC             EQU  H'001A'

;-----Bank1------------------
ADRES            EQU  H'009B'
ADRESL           EQU  H'009B'
ADRESH           EQU  H'009C'
ADCON0           EQU  H'009D'
ADCON1           EQU  H'009E'
ADACT            EQU  H'009F'

;-----Bank2------------------
RC1REG           EQU  H'0119'
RCREG            EQU  H'0119'
RCREG1           EQU  H'0119'
TX1REG           EQU  H'011A'
TXREG            EQU  H'011A'
TXREG1           EQU  H'011A'
SP1BRG           EQU  H'011B'
SP1BRGL          EQU  H'011B'
SPBRG            EQU  H'011B'
SPBRG1           EQU  H'011B'
SPBRGL           EQU  H'011B'
SP1BRGH          EQU  H'011C'
SPBRGH           EQU  H'011C'
SPBRGH1          EQU  H'011C'
RC1STA           EQU  H'011D'
RCSTA            EQU  H'011D'
RCSTA1           EQU  H'011D'
TX1STA           EQU  H'011E'
TXSTA            EQU  H'011E'
TXSTA1           EQU  H'011E'
BAUD1CON         EQU  H'011F'
BAUDCON          EQU  H'011F'
BAUDCON1         EQU  H'011F'
BAUDCTL          EQU  H'011F'
BAUDCTL1         EQU  H'011F'

;-----Bank3------------------
SSP1BUF          EQU  H'018C'
SSP1ADD          EQU  H'018D'
SSP1MSK          EQU  H'018E'
SSP1STAT         EQU  H'018F'
SSP1CON1         EQU  H'0190'
SSP1CON2         EQU  H'0191'
SSP1CON3         EQU  H'0192'
SSP2BUF          EQU  H'0196'
SSP2ADD          EQU  H'0197'
SSP2MSK          EQU  H'0198'
SSP2STAT         EQU  H'0199'
SSP2CON1         EQU  H'019A'
SSP2CON2         EQU  H'019B'
SSP2CON3         EQU  H'019C'

;-----Bank4------------------
TMR1L            EQU  H'020C'
TMR1H            EQU  H'020D'
T1CON            EQU  H'020E'
PR1              EQU  H'020F'
T1GCON           EQU  H'020F'
T1GATE           EQU  H'0210'
TMR1GATE         EQU  H'0210'
T1CLK            EQU  H'0211'
TMR1CLK          EQU  H'0211'

;-----Bank5------------------
T2TMR            EQU  H'028C'
TMR2             EQU  H'028C'
PR2              EQU  H'028D'
T2PR             EQU  H'028D'
T2CON            EQU  H'028E'
T2HLT            EQU  H'028F'
T2CLKCON         EQU  H'0290'
T2RST            EQU  H'0291'

;-----Bank6------------------
CCPR1            EQU  H'030C'
CCPR1L           EQU  H'030C'
CCPR1H           EQU  H'030D'
CCP1CON          EQU  H'030E'
CCP1CAP          EQU  H'030F'
CCPR2            EQU  H'0310'
CCPR2L           EQU  H'0310'
CCPR2H           EQU  H'0311'
CCP2CON          EQU  H'0312'
CCP2CAP          EQU  H'0313'
PWM3DCL          EQU  H'0314'
PWM3DCH          EQU  H'0315'
PWM3CON          EQU  H'0316'
PWM4DCL          EQU  H'0318'
PWM4DCH          EQU  H'0319'
PWM4CON          EQU  H'031A'
PWM5DCL          EQU  H'031C'
PWM5DCH          EQU  H'031D'
PWM5CON          EQU  H'031E'

;-----Bank7------------------
PWM6DCL          EQU  H'038C'
PWM6DCH          EQU  H'038D'
PWM6CON          EQU  H'038E'

;-----Bank11------------------
NCO1ACC          EQU  H'058C'
NCO1ACCL         EQU  H'058C'
NCO1ACCH         EQU  H'058D'
NCO1ACCU         EQU  H'058E'
NCO1INC          EQU  H'058F'
NCO1INCL         EQU  H'058F'
NCO1INCH         EQU  H'0590'
NCO1INCU         EQU  H'0591'
NCO1CON          EQU  H'0592'
NCO1CLK          EQU  H'0593'
TMR0             EQU  H'059C'
TMR0L            EQU  H'059C'
PR0              EQU  H'059D'
TMR0H            EQU  H'059D'
T0CON0           EQU  H'059E'
T0CON1           EQU  H'059F'

;-----Bank12------------------
CWG1CLKCON       EQU  H'060C'
CWG1DAT          EQU  H'060D'
CWG1DBR          EQU  H'060E'
CWG1DBF          EQU  H'060F'
CWG1CON0         EQU  H'0610'
CWG1CON1         EQU  H'0611'
CWG1AS0          EQU  H'0612'
CWG1AS1          EQU  H'0613'
CWG1STR          EQU  H'0614'

;-----Bank14------------------
PIR0             EQU  H'070C'
PIR1             EQU  H'070D'
PIR2             EQU  H'070E'
PIR3             EQU  H'070F'
PIR4             EQU  H'0710'
PIR5             EQU  H'0711'
PIR6             EQU  H'0712'
PIR7             EQU  H'0713'
PIE0             EQU  H'0716'
PIE1             EQU  H'0717'
PIE2             EQU  H'0718'
PIE3             EQU  H'0719'
PIE4             EQU  H'071A'
PIE5             EQU  H'071B'
PIE6             EQU  H'071C'
PIE7             EQU  H'071D'

;-----Bank15------------------
PMD0             EQU  H'0796'
PMD1             EQU  H'0797'
PMD2             EQU  H'0798'
PMD3             EQU  H'0799'
PMD4             EQU  H'079A'
PMD5             EQU  H'079B'

;-----Bank16------------------
WDTCON0          EQU  H'080C'
WDTCON1          EQU  H'080D'
WDTPSL           EQU  H'080E'
WDTPSH           EQU  H'080F'
WDTTMR           EQU  H'0810'
BORCON           EQU  H'0811'
VREGCON          EQU  H'0812'
PCON0            EQU  H'0813'
PCON1            EQU  H'0814'
NVMADR           EQU  H'081A'
NVMADRL          EQU  H'081A'
NVMADRH          EQU  H'081B'
NVMDAT           EQU  H'081C'
NVMDATL          EQU  H'081C'
NVMDATH          EQU  H'081D'
NVMCON1          EQU  H'081E'
NVMCON2          EQU  H'081F'

;-----Bank17------------------
CPUDOZE          EQU  H'088C'
OSCCON1          EQU  H'088D'
OSCCON2          EQU  H'088E'
OSCCON3          EQU  H'088F'
OSCSTAT          EQU  H'0890'
OSCEN            EQU  H'0891'
OSCTUNE          EQU  H'0892'
OSCFRQ           EQU  H'0893'
CLKRCON          EQU  H'0895'
CLKRCLK          EQU  H'0896'

;-----Bank18------------------
FVRCON           EQU  H'090C'
DAC1CON0         EQU  H'090E'
DAC1CON1         EQU  H'090F'
ZCDCON           EQU  H'091F'

;-----Bank19------------------
CMOUT            EQU  H'098F'
CMSTAT           EQU  H'098F'
CM1CON0          EQU  H'0990'
CM1CON1          EQU  H'0991'
CM1NCH           EQU  H'0992'
CM1PCH           EQU  H'0993'
CM2CON0          EQU  H'0994'
CM2CON1          EQU  H'0995'
CM2NCH           EQU  H'0996'
CM2PCH           EQU  H'0997'

;-----Bank20------------------
RC2REG           EQU  H'0A19'
RCREG2           EQU  H'0A19'
TX2REG           EQU  H'0A1A'
TXREG2           EQU  H'0A1A'
SP2BRG           EQU  H'0A1B'
SP2BRGL          EQU  H'0A1B'
SPBRG2           EQU  H'0A1B'
SP2BRGH          EQU  H'0A1C'
SPBRGH2          EQU  H'0A1C'
RC2STA           EQU  H'0A1D'
RCSTA2           EQU  H'0A1D'
TX2STA           EQU  H'0A1E'
TXSTA2           EQU  H'0A1E'
BAUD2CON         EQU  H'0A1F'
BAUDCON2         EQU  H'0A1F'
BAUDCTL2         EQU  H'0A1F'

;-----Bank60------------------
CLCDATA          EQU  H'1E0F'
CLC1CON          EQU  H'1E10'
CLC1POL          EQU  H'1E11'
CLC1SEL0         EQU  H'1E12'
CLC1SEL1         EQU  H'1E13'
CLC1SEL2         EQU  H'1E14'
CLC1SEL3         EQU  H'1E15'
CLC1GLS0         EQU  H'1E16'
CLC1GLS1         EQU  H'1E17'
CLC1GLS2         EQU  H'1E18'
CLC1GLS3         EQU  H'1E19'
CLC2CON          EQU  H'1E1A'
CLC2POL          EQU  H'1E1B'
CLC2SEL0         EQU  H'1E1C'
CLC2SEL1         EQU  H'1E1D'
CLC2SEL2         EQU  H'1E1E'
CLC2SEL3         EQU  H'1E1F'
CLC2GLS0         EQU  H'1E20'
CLC2GLS1         EQU  H'1E21'
CLC2GLS2         EQU  H'1E22'
CLC2GLS3         EQU  H'1E23'
CLC3CON          EQU  H'1E24'
CLC3POL          EQU  H'1E25'
CLC3SEL0         EQU  H'1E26'
CLC3SEL1         EQU  H'1E27'
CLC3SEL2         EQU  H'1E28'
CLC3SEL3         EQU  H'1E29'
CLC3GLS0         EQU  H'1E2A'
CLC3GLS1         EQU  H'1E2B'
CLC3GLS2         EQU  H'1E2C'
CLC3GLS3         EQU  H'1E2D'
CLC4CON          EQU  H'1E2E'
CLC4POL          EQU  H'1E2F'
CLC4SEL0         EQU  H'1E30'
CLC4SEL1         EQU  H'1E31'
CLC4SEL2         EQU  H'1E32'
CLC4SEL3         EQU  H'1E33'
CLC4GLS0         EQU  H'1E34'
CLC4GLS1         EQU  H'1E35'
CLC4GLS2         EQU  H'1E36'
CLC4GLS3         EQU  H'1E37'

;-----Bank61------------------
PPSLOCK          EQU  H'1E8F'
INTPPS           EQU  H'1E90'
T0CKIPPS         EQU  H'1E91'
T1CKIPPS         EQU  H'1E92'
T1GPPS           EQU  H'1E93'
T2INPPS          EQU  H'1E9C'
CCP1PPS          EQU  H'1EA1'
CCP2PPS          EQU  H'1EA2'
CWG1PPS          EQU  H'1EB1'
CLCIN0PPS        EQU  H'1EBB'
CLCIN1PPS        EQU  H'1EBC'
CLCIN2PPS        EQU  H'1EBD'
CLCIN3PPS        EQU  H'1EBE'
ADCACTPPS        EQU  H'1EC3'
SSP1CLKPPS       EQU  H'1EC5'
SSP1DATPPS       EQU  H'1EC6'
SSP1SSPPS        EQU  H'1EC7'
SSP2CLKPPS       EQU  H'1EC8'
SSP2DATPPS       EQU  H'1EC9'
SSP2SSPPS        EQU  H'1ECA'
RX1DTPPS         EQU  H'1ECB'
TX1CKPPS         EQU  H'1ECC'
RX2DTPPS         EQU  H'1ECD'
TX2CKPPS         EQU  H'1ECE'

;-----Bank62------------------
RA0PPS           EQU  H'1F10'
RA1PPS           EQU  H'1F11'
RA2PPS           EQU  H'1F12'
RA3PPS           EQU  H'1F13'
RA4PPS           EQU  H'1F14'
RA5PPS           EQU  H'1F15'
RA6PPS           EQU  H'1F16'
RA7PPS           EQU  H'1F17'
RB0PPS           EQU  H'1F18'
RB1PPS           EQU  H'1F19'
RB2PPS           EQU  H'1F1A'
RB3PPS           EQU  H'1F1B'
RB4PPS           EQU  H'1F1C'
RB5PPS           EQU  H'1F1D'
RB6PPS           EQU  H'1F1E'
RB7PPS           EQU  H'1F1F'
RC0PPS           EQU  H'1F20'
RC1PPS           EQU  H'1F21'
RC2PPS           EQU  H'1F22'
RC3PPS           EQU  H'1F23'
RC4PPS           EQU  H'1F24'
RC5PPS           EQU  H'1F25'
RC6PPS           EQU  H'1F26'
RC7PPS           EQU  H'1F27'
ANSELA           EQU  H'1F38'
WPUA             EQU  H'1F39'
ODCONA           EQU  H'1F3A'
SLRCONA          EQU  H'1F3B'
INLVLA           EQU  H'1F3C'
IOCAP            EQU  H'1F3D'
IOCAN            EQU  H'1F3E'
IOCAF            EQU  H'1F3F'
ANSELB           EQU  H'1F43'
WPUB             EQU  H'1F44'
ODCONB           EQU  H'1F45'
SLRCONB          EQU  H'1F46'
INLVLB           EQU  H'1F47'
IOCBP            EQU  H'1F48'
IOCBN            EQU  H'1F49'
IOCBF            EQU  H'1F4A'
ANSELC           EQU  H'1F4E'
WPUC             EQU  H'1F4F'
ODCONC           EQU  H'1F50'
SLRCONC          EQU  H'1F51'
INLVLC           EQU  H'1F52'
IOCCP            EQU  H'1F53'
IOCCN            EQU  H'1F54'
IOCCF            EQU  H'1F55'
WPUE             EQU  H'1F65'
INLVLE           EQU  H'1F68'
IOCEP            EQU  H'1F69'
IOCEN            EQU  H'1F6A'
IOCEF            EQU  H'1F6B'

;-----Bank63------------------
STATUS_SHAD      EQU  H'1FE4'
WREG_SHAD        EQU  H'1FE5'
BSR_SHAD         EQU  H'1FE6'
PCLATH_SHAD      EQU  H'1FE7'
FSR0L_SHAD       EQU  H'1FE8'
FSR0_SHAD        EQU  H'1FE8'
FSR0H_SHAD       EQU  H'1FE9'
FSR1L_SHAD       EQU  H'1FEA'
FSR1H_SHAD       EQU  H'1FEB'
STKPTR           EQU  H'1FED'
TOSL             EQU  H'1FEE'
TOSH             EQU  H'1FEF'

;----- STATUS Bits -----------------------------------------------------
C                EQU  H'0000'
DC               EQU  H'0001'
Z                EQU  H'0002'
NOT_PD           EQU  H'0003'
NOT_TO           EQU  H'0004'


;----- BSR Bits -----------------------------------------------------
BSR0             EQU  H'0000'
BSR1             EQU  H'0001'
BSR2             EQU  H'0002'
BSR3             EQU  H'0003'
BSR4             EQU  H'0004'
BSR5             EQU  H'0005'


;----- INTCON Bits -----------------------------------------------------
INTEDG           EQU  H'0000'
PEIE             EQU  H'0006'
GIE              EQU  H'0007'


;----- PORTA Bits -----------------------------------------------------
RA0              EQU  H'0000'
RA1              EQU  H'0001'
RA2              EQU  H'0002'
RA3              EQU  H'0003'
RA4              EQU  H'0004'
RA5              EQU  H'0005'
RA6              EQU  H'0006'
RA7              EQU  H'0007'


;----- PORTB Bits -----------------------------------------------------
RB0              EQU  H'0000'
RB1              EQU  H'0001'
RB2              EQU  H'0002'
RB3              EQU  H'0003'
RB4              EQU  H'0004'
RB5              EQU  H'0005'
RB6              EQU  H'0006'
RB7              EQU  H'0007'


;----- PORTC Bits -----------------------------------------------------
RC0              EQU  H'0000'
RC1              EQU  H'0001'
RC2              EQU  H'0002'
RC3              EQU  H'0003'
RC4              EQU  H'0004'
RC5              EQU  H'0005'
RC6              EQU  H'0006'
RC7              EQU  H'0007'


;----- PORTE Bits -----------------------------------------------------
RE3              EQU  H'0003'


;----- TRISA Bits -----------------------------------------------------
TRISA0           EQU  H'0000'
TRISA1           EQU  H'0001'
TRISA2           EQU  H'0002'
TRISA3           EQU  H'0003'
TRISA4           EQU  H'0004'
TRISA5           EQU  H'0005'
TRISA6           EQU  H'0006'
TRISA7           EQU  H'0007'


;----- TRISB Bits -----------------------------------------------------
TRISB0           EQU  H'0000'
TRISB1           EQU  H'0001'
TRISB2           EQU  H'0002'
TRISB3           EQU  H'0003'
TRISB4           EQU  H'0004'
TRISB5           EQU  H'0005'
TRISB6           EQU  H'0006'
TRISB7           EQU  H'0007'


;----- TRISC Bits -----------------------------------------------------
TRISC0           EQU  H'0000'
TRISC1           EQU  H'0001'
TRISC2           EQU  H'0002'
TRISC3           EQU  H'0003'
TRISC4           EQU  H'0004'
TRISC5           EQU  H'0005'
TRISC6           EQU  H'0006'
TRISC7           EQU  H'0007'


;----- LATA Bits -----------------------------------------------------
LATA0            EQU  H'0000'
LATA1            EQU  H'0001'
LATA2            EQU  H'0002'
LATA3            EQU  H'0003'
LATA4            EQU  H'0004'
LATA5            EQU  H'0005'
LATA6            EQU  H'0006'
LATA7            EQU  H'0007'


;----- LATB Bits -----------------------------------------------------
LATB0            EQU  H'0000'
LATB1            EQU  H'0001'
LATB2            EQU  H'0002'
LATB3            EQU  H'0003'
LATB4            EQU  H'0004'
LATB5            EQU  H'0005'
LATB6            EQU  H'0006'
LATB7            EQU  H'0007'


;----- LATC Bits -----------------------------------------------------
LATC0            EQU  H'0000'
LATC1            EQU  H'0001'
LATC2            EQU  H'0002'
LATC3            EQU  H'0003'
LATC4            EQU  H'0004'
LATC5            EQU  H'0005'
LATC6            EQU  H'0006'
LATC7            EQU  H'0007'


;----- ADRESL Bits -----------------------------------------------------
ADRESL0          EQU  H'0000'
ADRESL1          EQU  H'0001'
ADRESL2          EQU  H'0002'
ADRESL3          EQU  H'0003'
ADRESL4          EQU  H'0004'
ADRESL5          EQU  H'0005'
ADRESL6          EQU  H'0006'
ADRESL7          EQU  H'0007'


;----- ADRESH Bits -----------------------------------------------------
ADRESH0          EQU  H'0000'
ADRESH1          EQU  H'0001'
ADRESH2          EQU  H'0002'
ADRESH3          EQU  H'0003'
ADRESH4          EQU  H'0004'
ADRESH5          EQU  H'0005'
ADRESH6          EQU  H'0006'
ADRESH7          EQU  H'0007'


;----- ADCON0 Bits -----------------------------------------------------
ADON             EQU  H'0000'
GOnDONE          EQU  H'0001'
CHS0             EQU  H'0002'
CHS1             EQU  H'0003'
CHS2             EQU  H'0004'
CHS3             EQU  H'0005'
CHS4             EQU  H'0006'
CHS5             EQU  H'0007'



;----- ADCON1 Bits -----------------------------------------------------
ADPREF0          EQU  H'0000'
ADPREF1          EQU  H'0001'
ADCS0            EQU  H'0004'
ADCS1            EQU  H'0005'
ADCS2            EQU  H'0006'
ADFM             EQU  H'0007'



;----- ADACT Bits -----------------------------------------------------
ADACT0           EQU  H'0000'
ADACT1           EQU  H'0001'
ADACT2           EQU  H'0002'
ADACT3           EQU  H'0003'



;----- RC1STA Bits -----------------------------------------------------
RX9D             EQU  H'0000'
OERR             EQU  H'0001'
FERR             EQU  H'0002'
ADDEN            EQU  H'0003'
CREN             EQU  H'0004'
SREN             EQU  H'0005'
RX9              EQU  H'0006'
SPEN             EQU  H'0007'


;----- RCSTA Bits -----------------------------------------------------
RX9D             EQU  H'0000'
OERR             EQU  H'0001'
FERR             EQU  H'0002'
ADDEN            EQU  H'0003'
CREN             EQU  H'0004'
SREN             EQU  H'0005'
RX9              EQU  H'0006'
SPEN             EQU  H'0007'


;----- RCSTA1 Bits -----------------------------------------------------
RX9D             EQU  H'0000'
OERR             EQU  H'0001'
FERR             EQU  H'0002'
ADDEN            EQU  H'0003'
CREN             EQU  H'0004'
SREN             EQU  H'0005'
RX9              EQU  H'0006'
SPEN             EQU  H'0007'


;----- TX1STA Bits -----------------------------------------------------
TX9D             EQU  H'0000'
TRMT             EQU  H'0001'
BRGH             EQU  H'0002'
SENDB            EQU  H'0003'
SYNC_TX1STA      EQU  H'0004'
TXEN             EQU  H'0005'
TX9              EQU  H'0006'
CSRC             EQU  H'0007'


;----- TXSTA Bits -----------------------------------------------------
TX9D             EQU  H'0000'
TRMT             EQU  H'0001'
BRGH             EQU  H'0002'
SENDB            EQU  H'0003'
SYNC_TXSTA       EQU  H'0004'
TXEN             EQU  H'0005'
TX9              EQU  H'0006'
CSRC             EQU  H'0007'


;----- TXSTA1 Bits -----------------------------------------------------
TX9D             EQU  H'0000'
TRMT             EQU  H'0001'
BRGH             EQU  H'0002'
SENDB            EQU  H'0003'
SYNC_TXSTA1      EQU  H'0004'
TXEN             EQU  H'0005'
TX9              EQU  H'0006'
CSRC             EQU  H'0007'


;----- BAUD1CON Bits -----------------------------------------------------
ABDEN            EQU  H'0000'
WUE              EQU  H'0001'
BRG16            EQU  H'0003'
SCKP             EQU  H'0004'
RCIDL            EQU  H'0006'
ABDOVF           EQU  H'0007'


;----- BAUDCON Bits -----------------------------------------------------
ABDEN            EQU  H'0000'
WUE              EQU  H'0001'
BRG16            EQU  H'0003'
SCKP             EQU  H'0004'
RCIDL            EQU  H'0006'
ABDOVF           EQU  H'0007'


;----- BAUDCON1 Bits -----------------------------------------------------
ABDEN            EQU  H'0000'
WUE              EQU  H'0001'
BRG16            EQU  H'0003'
SCKP             EQU  H'0004'
RCIDL            EQU  H'0006'
ABDOVF           EQU  H'0007'


;----- BAUDCTL Bits -----------------------------------------------------
ABDEN            EQU  H'0000'
WUE              EQU  H'0001'
BRG16            EQU  H'0003'
SCKP             EQU  H'0004'
RCIDL            EQU  H'0006'
ABDOVF           EQU  H'0007'


;----- BAUDCTL1 Bits -----------------------------------------------------
ABDEN            EQU  H'0000'
WUE              EQU  H'0001'
BRG16            EQU  H'0003'
SCKP             EQU  H'0004'
RCIDL            EQU  H'0006'
ABDOVF           EQU  H'0007'


;----- SSP1MSK Bits -----------------------------------------------------
MSK0             EQU  H'0000'
MSK1             EQU  H'0001'
MSK2             EQU  H'0002'
MSK3             EQU  H'0003'
MSK4             EQU  H'0004'
MSK5             EQU  H'0005'
MSK6             EQU  H'0006'
MSK7             EQU  H'0007'


;----- SSP1STAT Bits -----------------------------------------------------
BF               EQU  H'0000'
UA               EQU  H'0001'
R_NOT_W          EQU  H'0002'
S                EQU  H'0003'
P                EQU  H'0004'
D_NOT_A          EQU  H'0005'
CKE              EQU  H'0006'
SMP              EQU  H'0007'

R_W              EQU  H'0002'
D_A              EQU  H'0005'

NOT_W            EQU  H'0002'
NOT_A            EQU  H'0005'

NOT_WRITE        EQU  H'0002'
NOT_ADDRESS      EQU  H'0005'

READ_WRITE       EQU  H'0002'
DATA_ADDRESS     EQU  H'0005'

I2C_READ         EQU  H'0002'
I2C_START        EQU  H'0003'
I2C_STOP         EQU  H'0004'
I2C_DAT          EQU  H'0005'


;----- SSP1CON1 Bits -----------------------------------------------------
CKP              EQU  H'0004'
SSPEN            EQU  H'0005'
SSPOV            EQU  H'0006'
WCOL             EQU  H'0007'

SSPM0            EQU  H'0000'
SSPM1            EQU  H'0001'
SSPM2            EQU  H'0002'
SSPM3            EQU  H'0003'


;----- SSP1CON2 Bits -----------------------------------------------------
SEN              EQU  H'0000'
RSEN             EQU  H'0001'
PEN              EQU  H'0002'
RCEN             EQU  H'0003'
ACKEN            EQU  H'0004'
ACKDT            EQU  H'0005'
ACKSTAT          EQU  H'0006'
GCEN             EQU  H'0007'


ADMSK1           EQU  H'0001'
ADMSK2           EQU  H'0002'
ADMSK3           EQU  H'0003'
ADMSK4           EQU  H'0004'
ADMSK5           EQU  H'0005'


;----- SSP1CON3 Bits -----------------------------------------------------
DHEN             EQU  H'0000'
AHEN             EQU  H'0001'
SBCDE            EQU  H'0002'
SDAHT            EQU  H'0003'
BOEN             EQU  H'0004'
SCIE             EQU  H'0005'
PCIE             EQU  H'0006'
ACKTIM           EQU  H'0007'


;----- SSP2MSK Bits -----------------------------------------------------
MSK0             EQU  H'0000'
MSK1             EQU  H'0001'
MSK2             EQU  H'0002'
MSK3             EQU  H'0003'
MSK4             EQU  H'0004'
MSK5             EQU  H'0005'
MSK6             EQU  H'0006'
MSK7             EQU  H'0007'


;----- SSP2STAT Bits -----------------------------------------------------
BF               EQU  H'0000'
UA               EQU  H'0001'
R_NOT_W          EQU  H'0002'
S                EQU  H'0003'
P                EQU  H'0004'
D_NOT_A          EQU  H'0005'
CKE              EQU  H'0006'
SMP              EQU  H'0007'

R_W              EQU  H'0002'
D_A              EQU  H'0005'

NOT_W            EQU  H'0002'
NOT_A            EQU  H'0005'

NOT_WRITE        EQU  H'0002'
NOT_ADDRESS      EQU  H'0005'

READ_WRITE       EQU  H'0002'
DATA_ADDRESS     EQU  H'0005'

I2C_READ         EQU  H'0002'
I2C_START        EQU  H'0003'
I2C_STOP         EQU  H'0004'
I2C_DAT          EQU  H'0005'


;----- SSP2CON1 Bits -----------------------------------------------------
CKP              EQU  H'0004'
SSPEN            EQU  H'0005'
SSPOV            EQU  H'0006'
WCOL             EQU  H'0007'

SSPM0            EQU  H'0000'
SSPM1            EQU  H'0001'
SSPM2            EQU  H'0002'
SSPM3            EQU  H'0003'


;----- SSP2CON2 Bits -----------------------------------------------------
SEN              EQU  H'0000'
RSEN             EQU  H'0001'
PEN              EQU  H'0002'
RCEN             EQU  H'0003'
ACKEN            EQU  H'0004'
ACKDT            EQU  H'0005'
ACKSTAT          EQU  H'0006'
GCEN             EQU  H'0007'


ADMSK1           EQU  H'0001'
ADMSK2           EQU  H'0002'
ADMSK3           EQU  H'0003'
ADMSK4           EQU  H'0004'
ADMSK5           EQU  H'0005'


;----- SSP2CON3 Bits -----------------------------------------------------
DHEN             EQU  H'0000'
AHEN             EQU  H'0001'
SBCDE            EQU  H'0002'
SDAHT            EQU  H'0003'
BOEN             EQU  H'0004'
SCIE             EQU  H'0005'
PCIE             EQU  H'0006'
ACKTIM           EQU  H'0007'


;----- TMR1L Bits -----------------------------------------------------
TMR1L0           EQU  H'0000'
TMR1L1           EQU  H'0001'
TMR1L2           EQU  H'0002'
TMR1L3           EQU  H'0003'
TMR1L4           EQU  H'0004'
TMR1L5           EQU  H'0005'
TMR1L6           EQU  H'0006'
TMR1L7           EQU  H'0007'



;----- TMR1H Bits -----------------------------------------------------
TMR1H0           EQU  H'0000'
TMR1H1           EQU  H'0001'
TMR1H2           EQU  H'0002'
TMR1H3           EQU  H'0003'
TMR1H4           EQU  H'0004'
TMR1H5           EQU  H'0005'
TMR1H6           EQU  H'0006'
TMR1H7           EQU  H'0007'



;----- T1CON Bits -----------------------------------------------------
ON_T1CON         EQU  H'0000'
RD16             EQU  H'0001'
NOT_SYNC         EQU  H'0002'

TMR1ON           EQU  H'0000'
T1RD16           EQU  H'0001'
NOT_T1SYNC       EQU  H'0002'
T1CKPS0          EQU  H'0004'
T1CKPS1          EQU  H'0005'

CKPS0            EQU  H'0004'
CKPS1            EQU  H'0005'


;----- PR1 Bits -----------------------------------------------------
GVAL             EQU  H'0002'
GGO_NOT_DONE     EQU  H'0003'
GSPM             EQU  H'0004'
GTM              EQU  H'0005'
GPOL             EQU  H'0006'
GE               EQU  H'0007'

T1GVAL           EQU  H'0002'
T1GGO_NOT_DONE   EQU  H'0003'
T1GSPM           EQU  H'0004'
T1GTM            EQU  H'0005'
T1GPOL           EQU  H'0006'
T1GE             EQU  H'0007'

T1GGO            EQU  H'0003'


;----- T1GCON Bits -----------------------------------------------------
GVAL             EQU  H'0002'
GGO_NOT_DONE     EQU  H'0003'
GSPM             EQU  H'0004'
GTM              EQU  H'0005'
GPOL             EQU  H'0006'
GE               EQU  H'0007'

T1GVAL           EQU  H'0002'
T1GGO_NOT_DONE   EQU  H'0003'
T1GSPM           EQU  H'0004'
T1GTM            EQU  H'0005'
T1GPOL           EQU  H'0006'
T1GE             EQU  H'0007'

T1GGO            EQU  H'0003'


;----- T1GATE Bits -----------------------------------------------------
GSS0             EQU  H'0000'
GSS1             EQU  H'0001'
GSS2             EQU  H'0002'
GSS3             EQU  H'0003'
GSS4             EQU  H'0004'

T1GSS0           EQU  H'0000'
T1GSS1           EQU  H'0001'
T1GSS2           EQU  H'0002'
T1GSS3           EQU  H'0003'
T1GSS4           EQU  H'0004'


;----- TMR1GATE Bits -----------------------------------------------------
GSS0             EQU  H'0000'
GSS1             EQU  H'0001'
GSS2             EQU  H'0002'
GSS3             EQU  H'0003'
GSS4             EQU  H'0004'

T1GSS0           EQU  H'0000'
T1GSS1           EQU  H'0001'
T1GSS2           EQU  H'0002'
T1GSS3           EQU  H'0003'
T1GSS4           EQU  H'0004'


;----- T1CLK Bits -----------------------------------------------------
T1CS0            EQU  H'0000'
T1CS1            EQU  H'0001'
T1CS2            EQU  H'0002'
T1CS3            EQU  H'0003'

CS0              EQU  H'0000'
CS1              EQU  H'0001'
CS2              EQU  H'0002'
CS3              EQU  H'0003'


;----- TMR1CLK Bits -----------------------------------------------------
T1CS0            EQU  H'0000'
T1CS1            EQU  H'0001'
T1CS2            EQU  H'0002'
T1CS3            EQU  H'0003'

CS0              EQU  H'0000'
CS1              EQU  H'0001'
CS2              EQU  H'0002'
CS3              EQU  H'0003'


;----- T2CON Bits -----------------------------------------------------
ON_T2CON         EQU  H'0007'

OUTPS0           EQU  H'0000'
OUTPS1           EQU  H'0001'
OUTPS2           EQU  H'0002'
OUTPS3           EQU  H'0003'
CKPS0            EQU  H'0004'
CKPS1            EQU  H'0005'
CKPS2            EQU  H'0006'

T2ON             EQU  H'0007'

T2OUTPS0         EQU  H'0000'
T2OUTPS1         EQU  H'0001'
T2OUTPS2         EQU  H'0002'
T2OUTPS3         EQU  H'0003'
T2CKPS0          EQU  H'0004'
T2CKPS1          EQU  H'0005'
T2CKPS2          EQU  H'0006'
TMR2ON           EQU  H'0007'


;----- T2HLT Bits -----------------------------------------------------
CKSYNC           EQU  H'0005'
CKPOL            EQU  H'0006'
PSYNC            EQU  H'0007'

MODE0            EQU  H'0000'
MODE1            EQU  H'0001'
MODE2            EQU  H'0002'
MODE3            EQU  H'0003'
MODE4            EQU  H'0004'

T2CKSYNC         EQU  H'0005'
T2CKPOL          EQU  H'0006'
T2PSYNC          EQU  H'0007'

T2MODE0          EQU  H'0000'
T2MODE1          EQU  H'0001'
T2MODE2          EQU  H'0002'
T2MODE3          EQU  H'0003'
T2MODE4          EQU  H'0004'


;----- T2CLKCON Bits -----------------------------------------------------
CS0              EQU  H'0000'
CS1              EQU  H'0001'
CS2              EQU  H'0002'
CS3              EQU  H'0003'


T2CS0            EQU  H'0000'
T2CS1            EQU  H'0001'
T2CS2            EQU  H'0002'
T2CS3            EQU  H'0003'


;----- T2RST Bits -----------------------------------------------------
RSEL0            EQU  H'0000'
RSEL1            EQU  H'0001'
RSEL2            EQU  H'0002'
RSEL3            EQU  H'0003'


T2RSEL0          EQU  H'0000'
T2RSEL1          EQU  H'0001'
T2RSEL2          EQU  H'0002'
T2RSEL3          EQU  H'0003'


;----- CCP1CON Bits -----------------------------------------------------
FMT              EQU  H'0004'
OUT_CCP1CON      EQU  H'0005'
OE               EQU  H'0006'
EN               EQU  H'0007'

MODE0            EQU  H'0000'
MODE1            EQU  H'0001'
MODE2            EQU  H'0002'
MODE3            EQU  H'0003'

CCP1FMT          EQU  H'0004'
CCP1OUT          EQU  H'0005'
CCP1OE           EQU  H'0006'
CCP1EN           EQU  H'0007'

CCP1MODE0        EQU  H'0000'
CCP1MODE1        EQU  H'0001'
CCP1MODE2        EQU  H'0002'
CCP1MODE3        EQU  H'0003'


;----- CCP1CAP Bits -----------------------------------------------------
CTS0             EQU  H'0000'
CTS1             EQU  H'0001'
CTS2             EQU  H'0002'


CCP1CTS0         EQU  H'0000'
CCP1CTS1         EQU  H'0001'
CCP1CTS2         EQU  H'0002'


;----- CCP2CON Bits -----------------------------------------------------
FMT              EQU  H'0004'
OUT_CCP2CON      EQU  H'0005'
OE               EQU  H'0006'
EN               EQU  H'0007'

MODE0            EQU  H'0000'
MODE1            EQU  H'0001'
MODE2            EQU  H'0002'
MODE3            EQU  H'0003'

CCP2FMT          EQU  H'0004'
CCP2OUT          EQU  H'0005'
CCP2OE           EQU  H'0006'
CCP2EN           EQU  H'0007'

CCP2MODE0        EQU  H'0000'
CCP2MODE1        EQU  H'0001'
CCP2MODE2        EQU  H'0002'
CCP2MODE3        EQU  H'0003'


;----- CCP2CAP Bits -----------------------------------------------------
CTS0             EQU  H'0000'
CTS1             EQU  H'0001'
CTS2             EQU  H'0002'


CCP2CTS0         EQU  H'0000'
CCP2CTS1         EQU  H'0001'
CCP2CTS2         EQU  H'0002'


;----- PWM3DCL Bits -----------------------------------------------------
DC0              EQU  H'0006'
DC1              EQU  H'0007'

PWM3DC0          EQU  H'0006'
PWM3DC1          EQU  H'0007'

PWMPW0           EQU  H'0006'
PWMPW1           EQU  H'0007'


;----- PWM3DCH Bits -----------------------------------------------------
DC2              EQU  H'0000'
DC3              EQU  H'0001'
DC4              EQU  H'0002'
DC5              EQU  H'0003'
DC6              EQU  H'0004'
DC7              EQU  H'0005'
DC8              EQU  H'0006'
DC9              EQU  H'0007'

PWM3DC2          EQU  H'0000'
PWM3DC3          EQU  H'0001'
PWM3DC4          EQU  H'0002'
PWM3DC5          EQU  H'0003'
PWM3DC6          EQU  H'0004'
PWM3DC7          EQU  H'0005'
PWM3DC8          EQU  H'0006'
PWM3DC9          EQU  H'0007'

PWMPW2           EQU  H'0000'
PWMPW3           EQU  H'0001'
PWMPW4           EQU  H'0002'
PWMPW5           EQU  H'0003'
PWMPW6           EQU  H'0004'
PWMPW7           EQU  H'0005'
PWMPW8           EQU  H'0006'
PWMPW9           EQU  H'0007'


;----- PWM3CON Bits -----------------------------------------------------
POL_PWM3CON      EQU  H'0004'
OUT_PWM3CON      EQU  H'0005'
OE               EQU  H'0006'
EN               EQU  H'0007'

PWM3POL          EQU  H'0004'
PWM3OUT          EQU  H'0005'
PWM3OE           EQU  H'0006'
PWM3EN           EQU  H'0007'


;----- PWM4DCL Bits -----------------------------------------------------
DC0              EQU  H'0006'
DC1              EQU  H'0007'

PWM4DC0          EQU  H'0006'
PWM4DC1          EQU  H'0007'

PWMPW0           EQU  H'0006'
PWMPW1           EQU  H'0007'


;----- PWM4DCH Bits -----------------------------------------------------
DC2              EQU  H'0000'
DC3              EQU  H'0001'
DC4              EQU  H'0002'
DC5              EQU  H'0003'
DC6              EQU  H'0004'
DC7              EQU  H'0005'
DC8              EQU  H'0006'
DC9              EQU  H'0007'

PWM4DC2          EQU  H'0000'
PWM4DC3          EQU  H'0001'
PWM4DC4          EQU  H'0002'
PWM4DC5          EQU  H'0003'
PWM4DC6          EQU  H'0004'
PWM4DC7          EQU  H'0005'
PWM4DC8          EQU  H'0006'
PWM4DC9          EQU  H'0007'

PWMPW2           EQU  H'0000'
PWMPW3           EQU  H'0001'
PWMPW4           EQU  H'0002'
PWMPW5           EQU  H'0003'
PWMPW6           EQU  H'0004'
PWMPW7           EQU  H'0005'
PWMPW8           EQU  H'0006'
PWMPW9           EQU  H'0007'


;----- PWM4CON Bits -----------------------------------------------------
POL_PWM4CON      EQU  H'0004'
OUT_PWM4CON      EQU  H'0005'
OE               EQU  H'0006'
EN               EQU  H'0007'

PWM4POL          EQU  H'0004'
PWM4OUT          EQU  H'0005'
PWM4OE           EQU  H'0006'
PWM4EN           EQU  H'0007'


;----- PWM5DCL Bits -----------------------------------------------------
DC0              EQU  H'0006'
DC1              EQU  H'0007'

PWM5DC0          EQU  H'0006'
PWM5DC1          EQU  H'0007'

PWMPW0           EQU  H'0006'
PWMPW1           EQU  H'0007'


;----- PWM5DCH Bits -----------------------------------------------------
DC2              EQU  H'0000'
DC3              EQU  H'0001'
DC4              EQU  H'0002'
DC5              EQU  H'0003'
DC6              EQU  H'0004'
DC7              EQU  H'0005'
DC8              EQU  H'0006'
DC9              EQU  H'0007'

PWM5DC2          EQU  H'0000'
PWM5DC3          EQU  H'0001'
PWM5DC4          EQU  H'0002'
PWM5DC5          EQU  H'0003'
PWM5DC6          EQU  H'0004'
PWM5DC7          EQU  H'0005'
PWM5DC8          EQU  H'0006'
PWM5DC9          EQU  H'0007'

PWMPW2           EQU  H'0000'
PWMPW3           EQU  H'0001'
PWMPW4           EQU  H'0002'
PWMPW5           EQU  H'0003'
PWMPW6           EQU  H'0004'
PWMPW7           EQU  H'0005'
PWMPW8           EQU  H'0006'
PWMPW9           EQU  H'0007'


;----- PWM5CON Bits -----------------------------------------------------
POL_PWM5CON      EQU  H'0004'
OUT_PWM5CON      EQU  H'0005'
OE               EQU  H'0006'
EN               EQU  H'0007'

PWM5POL          EQU  H'0004'
PWM5OUT          EQU  H'0005'
PWM5OE           EQU  H'0006'
PWM5EN           EQU  H'0007'


;----- PWM6DCL Bits -----------------------------------------------------
DC0              EQU  H'0006'
DC1              EQU  H'0007'

PWM6DC0          EQU  H'0006'
PWM6DC1          EQU  H'0007'

PWMPW0           EQU  H'0006'
PWMPW1           EQU  H'0007'


;----- PWM6DCH Bits -----------------------------------------------------
DC2              EQU  H'0000'
DC3              EQU  H'0001'
DC4              EQU  H'0002'
DC5              EQU  H'0003'
DC6              EQU  H'0004'
DC7              EQU  H'0005'
DC8              EQU  H'0006'
DC9              EQU  H'0007'

PWM6DC2          EQU  H'0000'
PWM6DC3          EQU  H'0001'
PWM6DC4          EQU  H'0002'
PWM6DC5          EQU  H'0003'
PWM6DC6          EQU  H'0004'
PWM6DC7          EQU  H'0005'
PWM6DC8          EQU  H'0006'
PWM6DC9          EQU  H'0007'

PWMPW2           EQU  H'0000'
PWMPW3           EQU  H'0001'
PWMPW4           EQU  H'0002'
PWMPW5           EQU  H'0003'
PWMPW6           EQU  H'0004'
PWMPW7           EQU  H'0005'
PWMPW8           EQU  H'0006'
PWMPW9           EQU  H'0007'


;----- PWM6CON Bits -----------------------------------------------------
POL_PWM6CON      EQU  H'0004'
OUT_PWM6CON      EQU  H'0005'
OE               EQU  H'0006'
EN               EQU  H'0007'

PWM6POL          EQU  H'0004'
PWM6OUT          EQU  H'0005'
PWM6OE           EQU  H'0006'
PWM6EN           EQU  H'0007'


;----- NCO1ACCL Bits -----------------------------------------------------
NCO1ACC0         EQU  H'0000'
NCO1ACC1         EQU  H'0001'
NCO1ACC2         EQU  H'0002'
NCO1ACC3         EQU  H'0003'
NCO1ACC4         EQU  H'0004'
NCO1ACC5         EQU  H'0005'
NCO1ACC6         EQU  H'0006'
NCO1ACC7         EQU  H'0007'



;----- NCO1ACCH Bits -----------------------------------------------------
NCO1ACC8         EQU  H'0000'
NCO1ACC9         EQU  H'0001'
NCO1ACC10        EQU  H'0002'
NCO1ACC11        EQU  H'0003'
NCO1ACC12        EQU  H'0004'
NCO1ACC13        EQU  H'0005'
NCO1ACC14        EQU  H'0006'
NCO1ACC15        EQU  H'0007'



;----- NCO1ACCU Bits -----------------------------------------------------
NCO1ACC16        EQU  H'0000'
NCO1ACC17        EQU  H'0001'
NCO1ACC18        EQU  H'0002'
NCO1ACC19        EQU  H'0003'



;----- NCO1INCL Bits -----------------------------------------------------
NCO1INC0         EQU  H'0000'
NCO1INC1         EQU  H'0001'
NCO1INC2         EQU  H'0002'
NCO1INC3         EQU  H'0003'
NCO1INC4         EQU  H'0004'
NCO1INC5         EQU  H'0005'
NCO1INC6         EQU  H'0006'
NCO1INC7         EQU  H'0007'



;----- NCO1INCH Bits -----------------------------------------------------
NCO1INC8         EQU  H'0000'
NCO1INC9         EQU  H'0001'
NCO1INC10        EQU  H'0002'
NCO1INC11        EQU  H'0003'
NCO1INC12        EQU  H'0004'
NCO1INC13        EQU  H'0005'
NCO1INC14        EQU  H'0006'
NCO1INC15        EQU  H'0007'



;----- NCO1INCU Bits -----------------------------------------------------
NCO1INC16        EQU  H'0000'
NCO1INC17        EQU  H'0001'
NCO1INC18        EQU  H'0002'
NCO1INC19        EQU  H'0003'



;----- NCO1CON Bits -----------------------------------------------------
N1PFM            EQU  H'0000'
N1POL            EQU  H'0004'
N1OUT            EQU  H'0005'
N1EN             EQU  H'0007'


;----- NCO1CLK Bits -----------------------------------------------------
N1CKS0           EQU  H'0000'
N1CKS1           EQU  H'0001'
N1CKS2           EQU  H'0002'
N1PWS0           EQU  H'0005'
N1PWS1           EQU  H'0006'
N1PWS2           EQU  H'0007'



;----- TMR0 Bits -----------------------------------------------------
TMR0L0           EQU  H'0000'
TMR0L1           EQU  H'0001'
TMR0L2           EQU  H'0002'
TMR0L3           EQU  H'0003'
TMR0L4           EQU  H'0004'
TMR0L5           EQU  H'0005'
TMR0L6           EQU  H'0006'
TMR0L7           EQU  H'0007'


;----- TMR0L Bits -----------------------------------------------------
TMR0L0           EQU  H'0000'
TMR0L1           EQU  H'0001'
TMR0L2           EQU  H'0002'
TMR0L3           EQU  H'0003'
TMR0L4           EQU  H'0004'
TMR0L5           EQU  H'0005'
TMR0L6           EQU  H'0006'
TMR0L7           EQU  H'0007'


;----- PR0 Bits -----------------------------------------------------
TMR0H0           EQU  H'0000'
TMR0H1           EQU  H'0001'
TMR0H2           EQU  H'0002'
TMR0H3           EQU  H'0003'
TMR0H4           EQU  H'0004'
TMR0H5           EQU  H'0005'
TMR0H6           EQU  H'0006'
TMR0H7           EQU  H'0007'

T0PR0            EQU  H'0000'
T0PR1            EQU  H'0001'
T0PR2            EQU  H'0002'
T0PR3            EQU  H'0003'
T0PR4            EQU  H'0004'
T0PR5            EQU  H'0005'
T0PR6            EQU  H'0006'
T0PR7            EQU  H'0007'


;----- TMR0H Bits -----------------------------------------------------
TMR0H0           EQU  H'0000'
TMR0H1           EQU  H'0001'
TMR0H2           EQU  H'0002'
TMR0H3           EQU  H'0003'
TMR0H4           EQU  H'0004'
TMR0H5           EQU  H'0005'
TMR0H6           EQU  H'0006'
TMR0H7           EQU  H'0007'

T0PR0            EQU  H'0000'
T0PR1            EQU  H'0001'
T0PR2            EQU  H'0002'
T0PR3            EQU  H'0003'
T0PR4            EQU  H'0004'
T0PR5            EQU  H'0005'
T0PR6            EQU  H'0006'
T0PR7            EQU  H'0007'


;----- T0CON0 Bits -----------------------------------------------------
T016BIT          EQU  H'0004'
T0OUT            EQU  H'0005'
T0OE             EQU  H'0006'
T0EN             EQU  H'0007'

T0OUTPS0         EQU  H'0000'
T0OUTPS1         EQU  H'0001'
T0OUTPS2         EQU  H'0002'
T0OUTPS3         EQU  H'0003'


;----- T0CON1 Bits -----------------------------------------------------
T0ASYNC          EQU  H'0004'

T0CKPS0          EQU  H'0000'
T0CKPS1          EQU  H'0001'
T0CKPS2          EQU  H'0002'
T0CKPS3          EQU  H'0003'
T0CS0            EQU  H'0005'
T0CS1            EQU  H'0006'
T0CS2            EQU  H'0007'

T0PS0            EQU  H'0000'
T0PS1            EQU  H'0001'
T0PS2            EQU  H'0002'
T0PS3            EQU  H'0003'



;----- CWG1CLKCON Bits -----------------------------------------------------
CS               EQU  H'0000'

CWG1CS           EQU  H'0000'


;----- CWG1DAT Bits -----------------------------------------------------
CWG1DAT0         EQU  H'0000'
CWG1DAT1         EQU  H'0001'
CWG1DAT2         EQU  H'0002'
CWG1DAT3         EQU  H'0003'


;----- CWG1DBR Bits -----------------------------------------------------
DBR0             EQU  H'0000'
DBR1             EQU  H'0001'
DBR2             EQU  H'0002'
DBR3             EQU  H'0003'
DBR4             EQU  H'0004'
DBR5             EQU  H'0005'


CWG1DBR0         EQU  H'0000'
CWG1DBR1         EQU  H'0001'
CWG1DBR2         EQU  H'0002'
CWG1DBR3         EQU  H'0003'
CWG1DBR4         EQU  H'0004'
CWG1DBR5         EQU  H'0005'


;----- CWG1DBF Bits -----------------------------------------------------
DBF0             EQU  H'0000'
DBF1             EQU  H'0001'
DBF2             EQU  H'0002'
DBF3             EQU  H'0003'
DBF4             EQU  H'0004'
DBF5             EQU  H'0005'


CWG1DBF0         EQU  H'0000'
CWG1DBF1         EQU  H'0001'
CWG1DBF2         EQU  H'0002'
CWG1DBF3         EQU  H'0003'
CWG1DBF4         EQU  H'0004'
CWG1DBF5         EQU  H'0005'


;----- CWG1CON0 Bits -----------------------------------------------------
LD               EQU  H'0006'
EN               EQU  H'0007'

MODE0            EQU  H'0000'
MODE1            EQU  H'0001'
MODE2            EQU  H'0002'
G1EN             EQU  H'0007'

CWG1LD           EQU  H'0006'
CWG1EN           EQU  H'0007'

CWG1MODE0        EQU  H'0000'
CWG1MODE1        EQU  H'0001'
CWG1MODE2        EQU  H'0002'


;----- CWG1CON1 Bits -----------------------------------------------------
POLA             EQU  H'0000'
POLB             EQU  H'0001'
POLC             EQU  H'0002'
POLD             EQU  H'0003'
IN               EQU  H'0005'

CWG1POLA         EQU  H'0000'
CWG1POLB         EQU  H'0001'
CWG1POLC         EQU  H'0002'
CWG1POLD         EQU  H'0003'
CWG1IN           EQU  H'0005'


;----- CWG1AS0 Bits -----------------------------------------------------
REN              EQU  H'0006'
SHUTDOWN         EQU  H'0007'

LSAC0            EQU  H'0002'
LSAC1            EQU  H'0003'
LSBD0            EQU  H'0004'
LSBD1            EQU  H'0005'

CWG1REN          EQU  H'0006'
CWG1SHUTDOWN     EQU  H'0007'

CWG1LSAC0        EQU  H'0002'
CWG1LSAC1        EQU  H'0003'
CWG1LSBD0        EQU  H'0004'
CWG1LSBD1        EQU  H'0005'


;----- CWG1AS1 Bits -----------------------------------------------------
AS0E             EQU  H'0000'
AS1E             EQU  H'0001'
AS2E             EQU  H'0002'
AS3E             EQU  H'0003'
AS4E             EQU  H'0004'


;----- CWG1STR Bits -----------------------------------------------------
STRA             EQU  H'0000'
STRB             EQU  H'0001'
STRC             EQU  H'0002'
STRD             EQU  H'0003'
OVRA             EQU  H'0004'
OVRB             EQU  H'0005'
OVRC             EQU  H'0006'
OVRD             EQU  H'0007'

CWG1STRA         EQU  H'0000'
CWG1STRB         EQU  H'0001'
CWG1STRC         EQU  H'0002'
CWG1STRD         EQU  H'0003'
CWG1OVRA         EQU  H'0004'
CWG1OVRB         EQU  H'0005'
CWG1OVRC         EQU  H'0006'
CWG1OVRD         EQU  H'0007'


;----- PIR0 Bits -----------------------------------------------------
INTF             EQU  H'0000'
IOCIF            EQU  H'0004'
TMR0IF           EQU  H'0005'


;----- PIR1 Bits -----------------------------------------------------
ADIF             EQU  H'0000'
ADTIF            EQU  H'0001'
CSWIF            EQU  H'0006'
OSFIF            EQU  H'0007'


;----- PIR2 Bits -----------------------------------------------------
C1IF             EQU  H'0000'
C2IF             EQU  H'0001'
ZCDIF            EQU  H'0006'


;----- PIR3 Bits -----------------------------------------------------
SSP1IF           EQU  H'0000'
BCL1IF           EQU  H'0001'
SSP2IF           EQU  H'0002'
BCL2IF           EQU  H'0003'
TX1IF            EQU  H'0004'
RC1IF            EQU  H'0005'
TX2IF            EQU  H'0006'
RC2IF            EQU  H'0007'


;----- PIR4 Bits -----------------------------------------------------
TMR1IF           EQU  H'0000'
TMR2IF           EQU  H'0001'


;----- PIR5 Bits -----------------------------------------------------
TMR1GIF          EQU  H'0000'
CLC1IF           EQU  H'0004'
CLC2IF           EQU  H'0005'
CLC3IF           EQU  H'0006'
CLC4IF           EQU  H'0007'


;----- PIR6 Bits -----------------------------------------------------
CCP1IF           EQU  H'0000'
CCP2IF           EQU  H'0001'


;----- PIR7 Bits -----------------------------------------------------
CWG1IF           EQU  H'0000'
NCO1IF           EQU  H'0004'
NVMIF            EQU  H'0005'

NCOIF            EQU  H'0004'


;----- PIE0 Bits -----------------------------------------------------
INTE             EQU  H'0000'
IOCIE            EQU  H'0004'
TMR0IE           EQU  H'0005'


;----- PIE1 Bits -----------------------------------------------------
ADIE             EQU  H'0000'
ADTIE            EQU  H'0001'
CSWIE            EQU  H'0006'
OSFIE            EQU  H'0007'


;----- PIE2 Bits -----------------------------------------------------
C1IE             EQU  H'0000'
C2IE             EQU  H'0001'
ZCDIE            EQU  H'0006'


;----- PIE3 Bits -----------------------------------------------------
SSP1IE           EQU  H'0000'
BCL1IE           EQU  H'0001'
SSP2IE           EQU  H'0002'
BCL2IE           EQU  H'0003'
TX1IE            EQU  H'0004'
RC1IE            EQU  H'0005'
TX2IE            EQU  H'0006'
RC2IE            EQU  H'0007'


;----- PIE4 Bits -----------------------------------------------------
TMR1IE           EQU  H'0000'
TMR2IE           EQU  H'0001'


;----- PIE5 Bits -----------------------------------------------------
TMR1GIE          EQU  H'0000'
CLC1IE           EQU  H'0004'
CLC2IE           EQU  H'0005'
CLC3IE           EQU  H'0006'
CLC4IE           EQU  H'0007'


;----- PIE6 Bits -----------------------------------------------------
CCP1IE           EQU  H'0000'
CCP2IE           EQU  H'0001'


;----- PIE7 Bits -----------------------------------------------------
CWG1IE           EQU  H'0000'
NCO1IE           EQU  H'0004'
NVMIE            EQU  H'0005'

NCOIE            EQU  H'0004'


;----- PMD0 Bits -----------------------------------------------------
IOCMD            EQU  H'0000'
CLKRMD           EQU  H'0001'
NVMMD            EQU  H'0002'
FVRMD            EQU  H'0006'
SYSCMD           EQU  H'0007'


;----- PMD1 Bits -----------------------------------------------------
TMR0MD           EQU  H'0000'
TMR1MD           EQU  H'0001'
TMR2MD           EQU  H'0002'
DDS1MD           EQU  H'0007'

NCOMD            EQU  H'0007'


;----- PMD2 Bits -----------------------------------------------------
ZCDMD            EQU  H'0000'
CMP1MD           EQU  H'0001'
CMP2MD           EQU  H'0002'
ADCMD            EQU  H'0006'
DAC1MD           EQU  H'0007'


;----- PMD3 Bits -----------------------------------------------------
CCP1MD           EQU  H'0000'
CCP2MD           EQU  H'0001'
PWM3MD           EQU  H'0002'
PWM4MD           EQU  H'0003'
PWM5MD           EQU  H'0004'
PWM6MD           EQU  H'0005'


;----- PMD4 Bits -----------------------------------------------------
CWG1MD           EQU  H'0000'
MSSP1MD          EQU  H'0004'
MSSP2MD          EQU  H'0005'
UART1MD          EQU  H'0006'
UART2MD          EQU  H'0007'


;----- PMD5 Bits -----------------------------------------------------
CLC1MD           EQU  H'0001'
CLC2MD           EQU  H'0002'
CLC3MD           EQU  H'0003'
CLC4MD           EQU  H'0004'


;----- WDTCON0 Bits -----------------------------------------------------
SEN              EQU  H'0000'

SWDTEN           EQU  H'0000'

WDTSEN           EQU  H'0000'

WDTPS0           EQU  H'0001'
WDTPS1           EQU  H'0002'
WDTPS2           EQU  H'0003'
WDTPS3           EQU  H'0004'
WDTPS4           EQU  H'0005'


;----- WDTCON1 Bits -----------------------------------------------------
WINDOW0          EQU  H'0000'
WINDOW1          EQU  H'0001'
WINDOW2          EQU  H'0002'


WDTWINDOW0       EQU  H'0000'
WDTWINDOW1       EQU  H'0001'
WDTWINDOW2       EQU  H'0002'
WDTCS0           EQU  H'0004'
WDTCS1           EQU  H'0005'
WDTCS2           EQU  H'0006'


;----- WDTPSL Bits -----------------------------------------------------
PSCNT0           EQU  H'0000'
PSCNT1           EQU  H'0001'
PSCNT2           EQU  H'0002'
PSCNT3           EQU  H'0003'
PSCNT4           EQU  H'0004'
PSCNT5           EQU  H'0005'
PSCNT6           EQU  H'0006'
PSCNT7           EQU  H'0007'


WDTPSCNT0        EQU  H'0000'
WDTPSCNT1        EQU  H'0001'
WDTPSCNT2        EQU  H'0002'
WDTPSCNT3        EQU  H'0003'
WDTPSCNT4        EQU  H'0004'
WDTPSCNT5        EQU  H'0005'
WDTPSCNT6        EQU  H'0006'
WDTPSCNT7        EQU  H'0007'


;----- WDTPSH Bits -----------------------------------------------------
PSCNT8           EQU  H'0000'
PSCNT9           EQU  H'0001'
PSCNT10          EQU  H'0002'
PSCNT11          EQU  H'0003'
PSCNT12          EQU  H'0004'
PSCNT13          EQU  H'0005'
PSCNT14          EQU  H'0006'
PSCNT15          EQU  H'0007'


WDTPSCNT8        EQU  H'0000'
WDTPSCNT9        EQU  H'0001'
WDTPSCNT10       EQU  H'0002'
WDTPSCNT11       EQU  H'0003'
WDTPSCNT12       EQU  H'0004'
WDTPSCNT13       EQU  H'0005'
WDTPSCNT14       EQU  H'0006'
WDTPSCNT15       EQU  H'0007'


;----- WDTTMR Bits -----------------------------------------------------
PSCNT16          EQU  H'0000'
PSCNT17          EQU  H'0001'
STATE            EQU  H'0002'

WDTPSCNT16       EQU  H'0000'
WDTPSCNT17       EQU  H'0001'
WDTSTATE         EQU  H'0002'
WDTTMR0          EQU  H'0003'
WDTTMR1          EQU  H'0004'
WDTTMR2          EQU  H'0005'
WDTTMR3          EQU  H'0006'
WDTTMR4          EQU  H'0007'


;----- BORCON Bits -----------------------------------------------------
BORRDY           EQU  H'0000'
SBOREN           EQU  H'0007'


;----- VREGCON Bits -----------------------------------------------------
VREGPM           EQU  H'0001'


;----- PCON0 Bits -----------------------------------------------------
NOT_BOR          EQU  H'0000'
NOT_POR          EQU  H'0001'
NOT_RI           EQU  H'0002'
NOT_RMCLR        EQU  H'0003'
NOT_RWDT         EQU  H'0004'
NOT_WDTWV        EQU  H'0005'
STKUNF           EQU  H'0006'
STKOVF           EQU  H'0007'


;----- PCON1 Bits -----------------------------------------------------
NOT_MEMV         EQU  H'0001'


;----- NVMADRL Bits -----------------------------------------------------
NVMADR0          EQU  H'0000'
NVMADR1          EQU  H'0001'
NVMADR2          EQU  H'0002'
NVMADR3          EQU  H'0003'
NVMADR4          EQU  H'0004'
NVMADR5          EQU  H'0005'
NVMADR6          EQU  H'0006'
NVMADR7          EQU  H'0007'


;----- NVMADRH Bits -----------------------------------------------------
NVMADR8          EQU  H'0000'
NVMADR9          EQU  H'0001'
NVMADR10         EQU  H'0002'
NVMADR11         EQU  H'0003'
NVMADR12         EQU  H'0004'
NVMADR13         EQU  H'0005'
NVMADR14         EQU  H'0006'


;----- NVMDATL Bits -----------------------------------------------------
NVMDAT0          EQU  H'0000'
NVMDAT1          EQU  H'0001'
NVMDAT2          EQU  H'0002'
NVMDAT3          EQU  H'0003'
NVMDAT4          EQU  H'0004'
NVMDAT5          EQU  H'0005'
NVMDAT6          EQU  H'0006'
NVMDAT7          EQU  H'0007'


;----- NVMDATH Bits -----------------------------------------------------
NVMDAT8          EQU  H'0000'
NVMDAT9          EQU  H'0001'
NVMDAT10         EQU  H'0002'
NVMDAT11         EQU  H'0003'
NVMDAT12         EQU  H'0004'
NVMDAT13         EQU  H'0005'


;----- NVMCON1 Bits -----------------------------------------------------
RD               EQU  H'0000'
WR               EQU  H'0001'
WREN             EQU  H'0002'
WRERR            EQU  H'0003'
FREE             EQU  H'0004'
LWLO             EQU  H'0005'
NVMREGS          EQU  H'0006'


;----- CPUDOZE Bits -----------------------------------------------------
DOZE0            EQU  H'0000'
DOZE1            EQU  H'0001'
DOZE2            EQU  H'0002'
DOE              EQU  H'0004'
ROI              EQU  H'0005'
DOZEN            EQU  H'0006'
IDLEN            EQU  H'0007'


;----- OSCCON1 Bits -----------------------------------------------------
NDIV0            EQU  H'0000'
NDIV1            EQU  H'0001'
NDIV2            EQU  H'0002'
NDIV3            EQU  H'0003'
NOSC0            EQU  H'0004'
NOSC1            EQU  H'0005'
NOSC2            EQU  H'0006'


;----- OSCCON2 Bits -----------------------------------------------------
CDIV0            EQU  H'0000'
CDIV1            EQU  H'0001'
CDIV2            EQU  H'0002'
CDIV3            EQU  H'0003'
COSC0            EQU  H'0004'
COSC1            EQU  H'0005'
COSC2            EQU  H'0006'


;----- OSCCON3 Bits -----------------------------------------------------
NOSCR            EQU  H'0003'
ORDY             EQU  H'0004'
SOSCPWR          EQU  H'0006'
CSWHOLD          EQU  H'0007'


;----- OSCSTAT Bits -----------------------------------------------------
PLLR             EQU  H'0000'
ADOR             EQU  H'0002'
SOR              EQU  H'0003'
LFOR             EQU  H'0004'
MFOR             EQU  H'0005'
HFOR             EQU  H'0006'
EXTOR            EQU  H'0007'


;----- OSCEN Bits -----------------------------------------------------
ADOEN            EQU  H'0002'
SOSCEN           EQU  H'0003'
LFOEN            EQU  H'0004'
MFOEN            EQU  H'0005'
HFOEN            EQU  H'0006'
EXTOEN           EQU  H'0007'


;----- OSCTUNE Bits -----------------------------------------------------
HFTUN0           EQU  H'0000'
HFTUN1           EQU  H'0001'
HFTUN2           EQU  H'0002'
HFTUN3           EQU  H'0003'
HFTUN4           EQU  H'0004'
HFTUN5           EQU  H'0005'


;----- OSCFRQ Bits -----------------------------------------------------
HFFRQ0           EQU  H'0000'
HFFRQ1           EQU  H'0001'
HFFRQ2           EQU  H'0002'


;----- CLKRCON Bits -----------------------------------------------------
CLKREN           EQU  H'0007'

CLKRDIV0         EQU  H'0000'
CLKRDIV1         EQU  H'0001'
CLKRDIV2         EQU  H'0002'
CLKRDC0          EQU  H'0003'
CLKRDC1          EQU  H'0004'


;----- CLKRCLK Bits -----------------------------------------------------
CLKRCLK0         EQU  H'0000'
CLKRCLK1         EQU  H'0001'
CLKRCLK2         EQU  H'0002'
CLKRCLK3         EQU  H'0003'


;----- FVRCON Bits -----------------------------------------------------
TSRNG            EQU  H'0004'
TSEN             EQU  H'0005'
FVRRDY           EQU  H'0006'
FVREN            EQU  H'0007'

ADFVR0           EQU  H'0000'
ADFVR1           EQU  H'0001'
CDAFVR0          EQU  H'0002'
CDAFVR1          EQU  H'0003'


;----- DAC1CON0 Bits -----------------------------------------------------
NSS              EQU  H'0000'
OE2              EQU  H'0004'
OE1              EQU  H'0005'
EN               EQU  H'0007'

DAC1NSS          EQU  H'0000'
DAC1PSS0         EQU  H'0002'
DAC1PSS1         EQU  H'0003'
DAC1OE2          EQU  H'0004'
DAC1OE1          EQU  H'0005'
DAC1EN           EQU  H'0007'

PSS0             EQU  H'0002'
PSS1             EQU  H'0003'


;----- DAC1CON1 Bits -----------------------------------------------------
DAC1R0           EQU  H'0000'
DAC1R1           EQU  H'0001'
DAC1R2           EQU  H'0002'
DAC1R3           EQU  H'0003'
DAC1R4           EQU  H'0004'


;----- ZCDCON Bits -----------------------------------------------------
ZCDINTN          EQU  H'0000'
ZCDINTP          EQU  H'0001'
ZCDPOL           EQU  H'0004'
ZCDOUT           EQU  H'0005'
ZCDSEN           EQU  H'0007'


;----- CMOUT Bits -----------------------------------------------------
MC1OUT           EQU  H'0000'
MC2OUT           EQU  H'0001'

C1OUT_CMOUT      EQU  H'0000'
C2OUT_CMOUT      EQU  H'0001'


;----- CMSTAT Bits -----------------------------------------------------
MC1OUT           EQU  H'0000'
MC2OUT           EQU  H'0001'

C1OUT_CMSTAT     EQU  H'0000'
C2OUT_CMSTAT     EQU  H'0001'


;----- CM1CON0 Bits -----------------------------------------------------
SYNC_CM1CON0     EQU  H'0000'
HYS              EQU  H'0001'
POL_CM1CON0      EQU  H'0004'
OUT_CM1CON0      EQU  H'0006'
EN               EQU  H'0007'

C1SYNC           EQU  H'0000'
C1HYS            EQU  H'0001'
C1POL            EQU  H'0004'
C1OUT_CM1CON0    EQU  H'0006'
C1EN             EQU  H'0007'


;----- CM1CON1 Bits -----------------------------------------------------
INTN_CM1CON1     EQU  H'0000'
INTP_CM1CON1     EQU  H'0001'

C1INTN           EQU  H'0000'
C1INTP           EQU  H'0001'


;----- CM1NCH Bits -----------------------------------------------------
NCH0             EQU  H'0000'
NCH1             EQU  H'0001'
NCH2             EQU  H'0002'

C1NCH0           EQU  H'0000'
C1NCH1           EQU  H'0001'
C1NCH2           EQU  H'0002'


;----- CM1PCH Bits -----------------------------------------------------
PCH0             EQU  H'0000'
PCH1             EQU  H'0001'
PCH2             EQU  H'0002'

C1PCH0           EQU  H'0000'
C1PCH1           EQU  H'0001'
C1PCH2           EQU  H'0002'


;----- CM2CON0 Bits -----------------------------------------------------
SYNC_CM2CON0     EQU  H'0000'
HYS              EQU  H'0001'
POL_CM2CON0      EQU  H'0004'
OUT_CM2CON0      EQU  H'0006'
EN               EQU  H'0007'

C2SYNC           EQU  H'0000'
C2HYS            EQU  H'0001'
C2POL            EQU  H'0004'
C2OUT_CM2CON0    EQU  H'0006'
C2EN             EQU  H'0007'


;----- CM2CON1 Bits -----------------------------------------------------
INTN_CM2CON1     EQU  H'0000'
INTP_CM2CON1     EQU  H'0001'

C2INTN           EQU  H'0000'
C2INTP           EQU  H'0001'


;----- CM2NCH Bits -----------------------------------------------------
NCH0             EQU  H'0000'
NCH1             EQU  H'0001'
NCH2             EQU  H'0002'

C2NCH0           EQU  H'0000'
C2NCH1           EQU  H'0001'
C2NCH2           EQU  H'0002'


;----- CM2PCH Bits -----------------------------------------------------
PCH0             EQU  H'0000'
PCH1             EQU  H'0001'
PCH2             EQU  H'0002'

C2PCH0           EQU  H'0000'
C2PCH1           EQU  H'0001'
C2PCH2           EQU  H'0002'


;----- RC2STA Bits -----------------------------------------------------
RX9D             EQU  H'0000'
OERR             EQU  H'0001'
FERR             EQU  H'0002'
ADDEN            EQU  H'0003'
CREN             EQU  H'0004'
SREN             EQU  H'0005'
RX9              EQU  H'0006'
SPEN             EQU  H'0007'


;----- RCSTA2 Bits -----------------------------------------------------
RX9D             EQU  H'0000'
OERR             EQU  H'0001'
FERR             EQU  H'0002'
ADDEN            EQU  H'0003'
CREN             EQU  H'0004'
SREN             EQU  H'0005'
RX9              EQU  H'0006'
SPEN             EQU  H'0007'


;----- TX2STA Bits -----------------------------------------------------
TX9D             EQU  H'0000'
TRMT             EQU  H'0001'
BRGH             EQU  H'0002'
SENDB            EQU  H'0003'
SYNC_TX2STA      EQU  H'0004'
TXEN             EQU  H'0005'
TX9              EQU  H'0006'
CSRC             EQU  H'0007'


;----- TXSTA2 Bits -----------------------------------------------------
TX9D             EQU  H'0000'
TRMT             EQU  H'0001'
BRGH             EQU  H'0002'
SENDB            EQU  H'0003'
SYNC_TXSTA2      EQU  H'0004'
TXEN             EQU  H'0005'
TX9              EQU  H'0006'
CSRC             EQU  H'0007'


;----- BAUD2CON Bits -----------------------------------------------------
ABDEN            EQU  H'0000'
WUE              EQU  H'0001'
BRG16            EQU  H'0003'
SCKP             EQU  H'0004'
RCIDL            EQU  H'0006'
ABDOVF           EQU  H'0007'


;----- BAUDCON2 Bits -----------------------------------------------------
ABDEN            EQU  H'0000'
WUE              EQU  H'0001'
BRG16            EQU  H'0003'
SCKP             EQU  H'0004'
RCIDL            EQU  H'0006'
ABDOVF           EQU  H'0007'


;----- BAUDCTL2 Bits -----------------------------------------------------
ABDEN            EQU  H'0000'
WUE              EQU  H'0001'
BRG16            EQU  H'0003'
SCKP             EQU  H'0004'
RCIDL            EQU  H'0006'
ABDOVF           EQU  H'0007'


;----- CLCDATA Bits -----------------------------------------------------
MLC1OUT          EQU  H'0000'
MLC2OUT          EQU  H'0001'
MLC3OUT          EQU  H'0002'
MLC4OUT          EQU  H'0003'


;----- CLC1CON Bits -----------------------------------------------------
LC1INTN          EQU  H'0003'
LC1INTP          EQU  H'0004'
LC1OUT           EQU  H'0005'
LC1EN            EQU  H'0007'

LC1MODE0         EQU  H'0000'
LC1MODE1         EQU  H'0001'
LC1MODE2         EQU  H'0002'

INTN_CLC1CON     EQU  H'0003'
INTP_CLC1CON     EQU  H'0004'
OUT_CLC1CON      EQU  H'0005'
EN               EQU  H'0007'

MODE0            EQU  H'0000'
MODE1            EQU  H'0001'
MODE2            EQU  H'0002'


;----- CLC1POL Bits -----------------------------------------------------
LC1G1POL         EQU  H'0000'
LC1G2POL         EQU  H'0001'
LC1G3POL         EQU  H'0002'
LC1G4POL         EQU  H'0003'
LC1POL           EQU  H'0007'

G1POL            EQU  H'0000'
G2POL            EQU  H'0001'
G3POL            EQU  H'0002'
G4POL            EQU  H'0003'
POL_CLC1POL      EQU  H'0007'


;----- CLC1SEL0 Bits -----------------------------------------------------
LC1D1S0          EQU  H'0000'
LC1D1S1          EQU  H'0001'
LC1D1S2          EQU  H'0002'
LC1D1S3          EQU  H'0003'
LC1D1S4          EQU  H'0004'
LC1D1S5          EQU  H'0005'



D1S0             EQU  H'0000'
D1S1             EQU  H'0001'
D1S2             EQU  H'0002'
D1S3             EQU  H'0003'
D1S4             EQU  H'0004'
D1S5             EQU  H'0005'


;----- CLC1SEL1 Bits -----------------------------------------------------
LC1D2S0          EQU  H'0000'
LC1D2S1          EQU  H'0001'
LC1D2S2          EQU  H'0002'
LC1D2S3          EQU  H'0003'
LC1D2S4          EQU  H'0004'
LC1D2S5          EQU  H'0005'



D2S0             EQU  H'0000'
D2S1             EQU  H'0001'
D2S2             EQU  H'0002'
D2S3             EQU  H'0003'
D2S4             EQU  H'0004'
D2S5             EQU  H'0005'


;----- CLC1SEL2 Bits -----------------------------------------------------
LC1D3S0          EQU  H'0000'
LC1D3S1          EQU  H'0001'
LC1D3S2          EQU  H'0002'
LC1D3S3          EQU  H'0003'
LC1D3S4          EQU  H'0004'
LC1D3S5          EQU  H'0005'



D3S0             EQU  H'0000'
D3S1             EQU  H'0001'
D3S2             EQU  H'0002'
D3S3             EQU  H'0003'
D3S4             EQU  H'0004'
D3S5             EQU  H'0005'


;----- CLC1SEL3 Bits -----------------------------------------------------
LC1D4S0          EQU  H'0000'
LC1D4S1          EQU  H'0001'
LC1D4S2          EQU  H'0002'
LC1D4S3          EQU  H'0003'
LC1D4S4          EQU  H'0004'
LC1D4S5          EQU  H'0005'



D4S0             EQU  H'0000'
D4S1             EQU  H'0001'
D4S2             EQU  H'0002'
D4S3             EQU  H'0003'
D4S4             EQU  H'0004'
D4S5             EQU  H'0005'


;----- CLC1GLS0 Bits -----------------------------------------------------
LC1G1D1N         EQU  H'0000'
LC1G1D1T         EQU  H'0001'
LC1G1D2N         EQU  H'0002'
LC1G1D2T         EQU  H'0003'
LC1G1D3N         EQU  H'0004'
LC1G1D3T         EQU  H'0005'
LC1G1D4N         EQU  H'0006'
LC1G1D4T         EQU  H'0007'

D1N              EQU  H'0000'
D1T              EQU  H'0001'
D2N              EQU  H'0002'
D2T              EQU  H'0003'
D3N              EQU  H'0004'
D3T              EQU  H'0005'
D4N              EQU  H'0006'
D4T              EQU  H'0007'


;----- CLC1GLS1 Bits -----------------------------------------------------
LC1G2D1N         EQU  H'0000'
LC1G2D1T         EQU  H'0001'
LC1G2D2N         EQU  H'0002'
LC1G2D2T         EQU  H'0003'
LC1G2D3N         EQU  H'0004'
LC1G2D3T         EQU  H'0005'
LC1G2D4N         EQU  H'0006'
LC1G2D4T         EQU  H'0007'

D1N              EQU  H'0000'
D1T              EQU  H'0001'
D2N              EQU  H'0002'
D2T              EQU  H'0003'
D3N              EQU  H'0004'
D3T              EQU  H'0005'
D4N              EQU  H'0006'
D4T              EQU  H'0007'


;----- CLC1GLS2 Bits -----------------------------------------------------
LC1G3D1N         EQU  H'0000'
LC1G3D1T         EQU  H'0001'
LC1G3D2N         EQU  H'0002'
LC1G3D2T         EQU  H'0003'
LC1G3D3N         EQU  H'0004'
LC1G3D3T         EQU  H'0005'
LC1G3D4N         EQU  H'0006'
LC1G3D4T         EQU  H'0007'

D1N              EQU  H'0000'
D1T              EQU  H'0001'
D2N              EQU  H'0002'
D2T              EQU  H'0003'
D3N              EQU  H'0004'
D3T              EQU  H'0005'
D4N              EQU  H'0006'
D4T              EQU  H'0007'


;----- CLC1GLS3 Bits -----------------------------------------------------
LC1G4D1N         EQU  H'0000'
LC1G4D1T         EQU  H'0001'
LC1G4D2N         EQU  H'0002'
LC1G4D2T         EQU  H'0003'
LC1G4D3N         EQU  H'0004'
LC1G4D3T         EQU  H'0005'
LC1G4D4N         EQU  H'0006'
LC1G4D4T         EQU  H'0007'

G4D1N            EQU  H'0000'
G4D1T            EQU  H'0001'
G4D2N            EQU  H'0002'
G4D2T            EQU  H'0003'
G4D3N            EQU  H'0004'
G4D3T            EQU  H'0005'
G4D4N            EQU  H'0006'
G4D4T            EQU  H'0007'


;----- CLC2CON Bits -----------------------------------------------------
LC2INTN          EQU  H'0003'
LC2INTP          EQU  H'0004'
LC2OUT           EQU  H'0005'
LC2EN            EQU  H'0007'

LC2MODE0         EQU  H'0000'
LC2MODE1         EQU  H'0001'
LC2MODE2         EQU  H'0002'

INTN_CLC2CON     EQU  H'0003'
INTP_CLC2CON     EQU  H'0004'
OUT_CLC2CON      EQU  H'0005'
EN               EQU  H'0007'

MODE0            EQU  H'0000'
MODE1            EQU  H'0001'
MODE2            EQU  H'0002'


;----- CLC2POL Bits -----------------------------------------------------
LC2G1POL         EQU  H'0000'
LC2G2POL         EQU  H'0001'
LC2G3POL         EQU  H'0002'
LC2G4POL         EQU  H'0003'
LC2POL           EQU  H'0007'

G1POL            EQU  H'0000'
G2POL            EQU  H'0001'
G3POL            EQU  H'0002'
G4POL            EQU  H'0003'
POL_CLC2POL      EQU  H'0007'


;----- CLC2SEL0 Bits -----------------------------------------------------
LC2D1S0          EQU  H'0000'
LC2D1S1          EQU  H'0001'
LC2D1S2          EQU  H'0002'
LC2D1S3          EQU  H'0003'
LC2D1S4          EQU  H'0004'
LC2D1S5          EQU  H'0005'



D1S0             EQU  H'0000'
D1S1             EQU  H'0001'
D1S2             EQU  H'0002'
D1S3             EQU  H'0003'
D1S4             EQU  H'0004'
D1S5             EQU  H'0005'


;----- CLC2SEL1 Bits -----------------------------------------------------
LC2D2S0          EQU  H'0000'
LC2D2S1          EQU  H'0001'
LC2D2S2          EQU  H'0002'
LC2D2S3          EQU  H'0003'
LC2D2S4          EQU  H'0004'
LC2D2S5          EQU  H'0005'



D2S0             EQU  H'0000'
D2S1             EQU  H'0001'
D2S2             EQU  H'0002'
D2S3             EQU  H'0003'
D2S4             EQU  H'0004'
D2S5             EQU  H'0005'


;----- CLC2SEL2 Bits -----------------------------------------------------
LC2D3S0          EQU  H'0000'
LC2D3S1          EQU  H'0001'
LC2D3S2          EQU  H'0002'
LC2D3S3          EQU  H'0003'
LC2D3S4          EQU  H'0004'
LC2D3S5          EQU  H'0005'



D3S0             EQU  H'0000'
D3S1             EQU  H'0001'
D3S2             EQU  H'0002'
D3S3             EQU  H'0003'
D3S4             EQU  H'0004'
D3S5             EQU  H'0005'


;----- CLC2SEL3 Bits -----------------------------------------------------
LC2D4S0          EQU  H'0000'
LC2D4S1          EQU  H'0001'
LC2D4S2          EQU  H'0002'
LC2D4S3          EQU  H'0003'
LC2D4S4          EQU  H'0004'
LC2D4S5          EQU  H'0005'



D4S0             EQU  H'0000'
D4S1             EQU  H'0001'
D4S2             EQU  H'0002'
D4S3             EQU  H'0003'
D4S4             EQU  H'0004'
D4S5             EQU  H'0005'


;----- CLC2GLS0 Bits -----------------------------------------------------
LC2G1D1N         EQU  H'0000'
LC2G1D1T         EQU  H'0001'
LC2G1D2N         EQU  H'0002'
LC2G1D2T         EQU  H'0003'
LC2G1D3N         EQU  H'0004'
LC2G1D3T         EQU  H'0005'
LC2G1D4N         EQU  H'0006'
LC2G1D4T         EQU  H'0007'

D1N              EQU  H'0000'
D1T              EQU  H'0001'
D2N              EQU  H'0002'
D2T              EQU  H'0003'
D3N              EQU  H'0004'
D3T              EQU  H'0005'
D4N              EQU  H'0006'
D4T              EQU  H'0007'


;----- CLC2GLS1 Bits -----------------------------------------------------
LC2G2D1N         EQU  H'0000'
LC2G2D1T         EQU  H'0001'
LC2G2D2N         EQU  H'0002'
LC2G2D2T         EQU  H'0003'
LC2G2D3N         EQU  H'0004'
LC2G2D3T         EQU  H'0005'
LC2G2D4N         EQU  H'0006'
LC2G2D4T         EQU  H'0007'

D1N              EQU  H'0000'
D1T              EQU  H'0001'
D2N              EQU  H'0002'
D2T              EQU  H'0003'
D3N              EQU  H'0004'
D3T              EQU  H'0005'
D4N              EQU  H'0006'
D4T              EQU  H'0007'


;----- CLC2GLS2 Bits -----------------------------------------------------
LC2G3D1N         EQU  H'0000'
LC2G3D1T         EQU  H'0001'
LC2G3D2N         EQU  H'0002'
LC2G3D2T         EQU  H'0003'
LC2G3D3N         EQU  H'0004'
LC2G3D3T         EQU  H'0005'
LC2G3D4N         EQU  H'0006'
LC2G3D4T         EQU  H'0007'

D1N              EQU  H'0000'
D1T              EQU  H'0001'
D2N              EQU  H'0002'
D2T              EQU  H'0003'
D3N              EQU  H'0004'
D3T              EQU  H'0005'
D4N              EQU  H'0006'
D4T              EQU  H'0007'


;----- CLC2GLS3 Bits -----------------------------------------------------
LC2G4D1N         EQU  H'0000'
LC2G4D1T         EQU  H'0001'
LC2G4D2N         EQU  H'0002'
LC2G4D2T         EQU  H'0003'
LC2G4D3N         EQU  H'0004'
LC2G4D3T         EQU  H'0005'
LC2G4D4N         EQU  H'0006'
LC2G4D4T         EQU  H'0007'

G4D1N            EQU  H'0000'
G4D1T            EQU  H'0001'
G4D2N            EQU  H'0002'
G4D2T            EQU  H'0003'
G4D3N            EQU  H'0004'
G4D3T            EQU  H'0005'
G4D4N            EQU  H'0006'
G4D4T            EQU  H'0007'


;----- CLC3CON Bits -----------------------------------------------------
LC3INTN          EQU  H'0003'
LC3INTP          EQU  H'0004'
LC3OUT           EQU  H'0005'
LC3EN            EQU  H'0007'

LC3MODE0         EQU  H'0000'
LC3MODE1         EQU  H'0001'
LC3MODE2         EQU  H'0002'

INTN_CLC3CON     EQU  H'0003'
INTP_CLC3CON     EQU  H'0004'
OUT_CLC3CON      EQU  H'0005'
EN               EQU  H'0007'

MODE0            EQU  H'0000'
MODE1            EQU  H'0001'
MODE2            EQU  H'0002'


;----- CLC3POL Bits -----------------------------------------------------
LC3G1POL         EQU  H'0000'
LC3G2POL         EQU  H'0001'
LC3G3POL         EQU  H'0002'
LC3G4POL         EQU  H'0003'
LC3POL           EQU  H'0007'

G1POL            EQU  H'0000'
G2POL            EQU  H'0001'
G3POL            EQU  H'0002'
G4POL            EQU  H'0003'
POL_CLC3POL      EQU  H'0007'


;----- CLC3SEL0 Bits -----------------------------------------------------
LC3D1S0          EQU  H'0000'
LC3D1S1          EQU  H'0001'
LC3D1S2          EQU  H'0002'
LC3D1S3          EQU  H'0003'
LC3D1S4          EQU  H'0004'
LC3D1S5          EQU  H'0005'



D1S0             EQU  H'0000'
D1S1             EQU  H'0001'
D1S2             EQU  H'0002'
D1S3             EQU  H'0003'
D1S4             EQU  H'0004'
D1S5             EQU  H'0005'


;----- CLC3SEL1 Bits -----------------------------------------------------
LC3D2S0          EQU  H'0000'
LC3D2S1          EQU  H'0001'
LC3D2S2          EQU  H'0002'
LC3D2S3          EQU  H'0003'
LC3D2S4          EQU  H'0004'
LC3D2S5          EQU  H'0005'



D2S0             EQU  H'0000'
D2S1             EQU  H'0001'
D2S2             EQU  H'0002'
D2S3             EQU  H'0003'
D2S4             EQU  H'0004'
D2S5             EQU  H'0005'


;----- CLC3SEL2 Bits -----------------------------------------------------
LC3D3S0          EQU  H'0000'
LC3D3S1          EQU  H'0001'
LC3D3S2          EQU  H'0002'
LC3D3S3          EQU  H'0003'
LC3D3S4          EQU  H'0004'
LC3D3S5          EQU  H'0005'



D3S0             EQU  H'0000'
D3S1             EQU  H'0001'
D3S2             EQU  H'0002'
D3S3             EQU  H'0003'
D3S4             EQU  H'0004'
D3S5             EQU  H'0005'


;----- CLC3SEL3 Bits -----------------------------------------------------
LC3D4S0          EQU  H'0000'
LC3D4S1          EQU  H'0001'
LC3D4S2          EQU  H'0002'
LC3D4S3          EQU  H'0003'
LC3D4S4          EQU  H'0004'
LC3D4S5          EQU  H'0005'



D4S0             EQU  H'0000'
D4S1             EQU  H'0001'
D4S2             EQU  H'0002'
D4S3             EQU  H'0003'
D4S4             EQU  H'0004'
D4S5             EQU  H'0005'


;----- CLC3GLS0 Bits -----------------------------------------------------
LC3G1D1N         EQU  H'0000'
LC3G1D1T         EQU  H'0001'
LC3G1D2N         EQU  H'0002'
LC3G1D2T         EQU  H'0003'
LC3G1D3N         EQU  H'0004'
LC3G1D3T         EQU  H'0005'
LC3G1D4N         EQU  H'0006'
LC3G1D4T         EQU  H'0007'

D1N              EQU  H'0000'
D1T              EQU  H'0001'
D2N              EQU  H'0002'
D2T              EQU  H'0003'
D3N              EQU  H'0004'
D3T              EQU  H'0005'
D4N              EQU  H'0006'
D4T              EQU  H'0007'


;----- CLC3GLS1 Bits -----------------------------------------------------
LC3G2D1N         EQU  H'0000'
LC3G2D1T         EQU  H'0001'
LC3G2D2N         EQU  H'0002'
LC3G2D2T         EQU  H'0003'
LC3G2D3N         EQU  H'0004'
LC3G2D3T         EQU  H'0005'
LC3G2D4N         EQU  H'0006'
LC3G2D4T         EQU  H'0007'

D1N              EQU  H'0000'
D1T              EQU  H'0001'
D2N              EQU  H'0002'
D2T              EQU  H'0003'
D3N              EQU  H'0004'
D3T              EQU  H'0005'
D4N              EQU  H'0006'
D4T              EQU  H'0007'


;----- CLC3GLS2 Bits -----------------------------------------------------
LC3G3D1N         EQU  H'0000'
LC3G3D1T         EQU  H'0001'
LC3G3D2N         EQU  H'0002'
LC3G3D2T         EQU  H'0003'
LC3G3D3N         EQU  H'0004'
LC3G3D3T         EQU  H'0005'
LC3G3D4N         EQU  H'0006'
LC3G3D4T         EQU  H'0007'

D1N              EQU  H'0000'
D1T              EQU  H'0001'
D2N              EQU  H'0002'
D2T              EQU  H'0003'
D3N              EQU  H'0004'
D3T              EQU  H'0005'
D4N              EQU  H'0006'
D4T              EQU  H'0007'


;----- CLC3GLS3 Bits -----------------------------------------------------
LC3G4D1N         EQU  H'0000'
LC3G4D1T         EQU  H'0001'
LC3G4D2N         EQU  H'0002'
LC3G4D2T         EQU  H'0003'
LC3G4D3N         EQU  H'0004'
LC3G4D3T         EQU  H'0005'
LC3G4D4N         EQU  H'0006'
LC3G4D4T         EQU  H'0007'

G4D1N            EQU  H'0000'
G4D1T            EQU  H'0001'
G4D2N            EQU  H'0002'
G4D2T            EQU  H'0003'
G4D3N            EQU  H'0004'
G4D3T            EQU  H'0005'
G4D4N            EQU  H'0006'
G4D4T            EQU  H'0007'


;----- CLC4CON Bits -----------------------------------------------------
LC4INTN          EQU  H'0003'
LC4INTP          EQU  H'0004'
LC4OUT           EQU  H'0005'
LC4EN            EQU  H'0007'

LC4MODE0         EQU  H'0000'
LC4MODE1         EQU  H'0001'
LC4MODE2         EQU  H'0002'

INTN_CLC4CON     EQU  H'0003'
INTP_CLC4CON     EQU  H'0004'
OUT_CLC4CON      EQU  H'0005'
EN               EQU  H'0007'

MODE0            EQU  H'0000'
MODE1            EQU  H'0001'
MODE2            EQU  H'0002'


;----- CLC4POL Bits -----------------------------------------------------
LC4G1POL         EQU  H'0000'
LC4G2POL         EQU  H'0001'
LC4G3POL         EQU  H'0002'
LC4G4POL         EQU  H'0003'
LC4POL           EQU  H'0007'

G1POL            EQU  H'0000'
G2POL            EQU  H'0001'
G3POL            EQU  H'0002'
G4POL            EQU  H'0003'
POL_CLC4POL      EQU  H'0007'


;----- CLC4SEL0 Bits -----------------------------------------------------
LC4D1S0          EQU  H'0000'
LC4D1S1          EQU  H'0001'
LC4D1S2          EQU  H'0002'
LC4D1S3          EQU  H'0003'
LC4D1S4          EQU  H'0004'
LC4D1S5          EQU  H'0005'



D1S0             EQU  H'0000'
D1S1             EQU  H'0001'
D1S2             EQU  H'0002'
D1S3             EQU  H'0003'
D1S4             EQU  H'0004'
D1S5             EQU  H'0005'


;----- CLC4SEL1 Bits -----------------------------------------------------
LC4D2S0          EQU  H'0000'
LC4D2S1          EQU  H'0001'
LC4D2S2          EQU  H'0002'
LC4D2S3          EQU  H'0003'
LC4D2S4          EQU  H'0004'
LC4D2S5          EQU  H'0005'



D2S0             EQU  H'0000'
D2S1             EQU  H'0001'
D2S2             EQU  H'0002'
D2S3             EQU  H'0003'
D2S4             EQU  H'0004'
D2S5             EQU  H'0005'


;----- CLC4SEL2 Bits -----------------------------------------------------
LC4D3S0          EQU  H'0000'
LC4D3S1          EQU  H'0001'
LC4D3S2          EQU  H'0002'
LC4D3S3          EQU  H'0003'
LC4D3S4          EQU  H'0004'
LC4D3S5          EQU  H'0005'



D3S0             EQU  H'0000'
D3S1             EQU  H'0001'
D3S2             EQU  H'0002'
D3S3             EQU  H'0003'
D3S4             EQU  H'0004'
D3S5             EQU  H'0005'


;----- CLC4SEL3 Bits -----------------------------------------------------
LC4D4S0          EQU  H'0000'
LC4D4S1          EQU  H'0001'
LC4D4S2          EQU  H'0002'
LC4D4S3          EQU  H'0003'
LC4D4S4          EQU  H'0004'
LC4D4S5          EQU  H'0005'



D4S0             EQU  H'0000'
D4S1             EQU  H'0001'
D4S2             EQU  H'0002'
D4S3             EQU  H'0003'
D4S4             EQU  H'0004'
D4S5             EQU  H'0005'


;----- CLC4GLS0 Bits -----------------------------------------------------
LC4G1D1N         EQU  H'0000'
LC4G1D1T         EQU  H'0001'
LC4G1D2N         EQU  H'0002'
LC4G1D2T         EQU  H'0003'
LC4G1D3N         EQU  H'0004'
LC4G1D3T         EQU  H'0005'
LC4G1D4N         EQU  H'0006'
LC4G1D4T         EQU  H'0007'

D1N              EQU  H'0000'
D1T              EQU  H'0001'
D2N              EQU  H'0002'
D2T              EQU  H'0003'
D3N              EQU  H'0004'
D3T              EQU  H'0005'
D4N              EQU  H'0006'
D4T              EQU  H'0007'


;----- CLC4GLS1 Bits -----------------------------------------------------
LC4G2D1N         EQU  H'0000'
LC4G2D1T         EQU  H'0001'
LC4G2D2N         EQU  H'0002'
LC4G2D2T         EQU  H'0003'
LC4G2D3N         EQU  H'0004'
LC4G2D3T         EQU  H'0005'
LC4G2D4N         EQU  H'0006'
LC4G2D4T         EQU  H'0007'

D1N              EQU  H'0000'
D1T              EQU  H'0001'
D2N              EQU  H'0002'
D2T              EQU  H'0003'
D3N              EQU  H'0004'
D3T              EQU  H'0005'
D4N              EQU  H'0006'
D4T              EQU  H'0007'


;----- CLC4GLS2 Bits -----------------------------------------------------
LC4G3D1N         EQU  H'0000'
LC4G3D1T         EQU  H'0001'
LC4G3D2N         EQU  H'0002'
LC4G3D2T         EQU  H'0003'
LC4G3D3N         EQU  H'0004'
LC4G3D3T         EQU  H'0005'
LC4G3D4N         EQU  H'0006'
LC4G3D4T         EQU  H'0007'

D1N              EQU  H'0000'
D1T              EQU  H'0001'
D2N              EQU  H'0002'
D2T              EQU  H'0003'
D3N              EQU  H'0004'
D3T              EQU  H'0005'
D4N              EQU  H'0006'
D4T              EQU  H'0007'


;----- CLC4GLS3 Bits -----------------------------------------------------
LC4G4D1N         EQU  H'0000'
LC4G4D1T         EQU  H'0001'
LC4G4D2N         EQU  H'0002'
LC4G4D2T         EQU  H'0003'
LC4G4D3N         EQU  H'0004'
LC4G4D3T         EQU  H'0005'
LC4G4D4N         EQU  H'0006'
LC4G4D4T         EQU  H'0007'

G4D1N            EQU  H'0000'
G4D1T            EQU  H'0001'
G4D2N            EQU  H'0002'
G4D2T            EQU  H'0003'
G4D3N            EQU  H'0004'
G4D3T            EQU  H'0005'
G4D4N            EQU  H'0006'
G4D4T            EQU  H'0007'


;----- PPSLOCK Bits -----------------------------------------------------
PPSLOCKED        EQU  H'0000'


;----- INTPPS Bits -----------------------------------------------------
INTPPS0          EQU  H'0000'
INTPPS1          EQU  H'0001'
INTPPS2          EQU  H'0002'
INTPPS3          EQU  H'0003'


;----- T0CKIPPS Bits -----------------------------------------------------
T0CKIPPS0        EQU  H'0000'
T0CKIPPS1        EQU  H'0001'
T0CKIPPS2        EQU  H'0002'
T0CKIPPS3        EQU  H'0003'


;----- T1CKIPPS Bits -----------------------------------------------------
T1CKIPPS0        EQU  H'0000'
T1CKIPPS1        EQU  H'0001'
T1CKIPPS2        EQU  H'0002'
T1CKIPPS3        EQU  H'0003'
T1CKIPPS4        EQU  H'0004'


;----- T1GPPS Bits -----------------------------------------------------
T1GPPS0          EQU  H'0000'
T1GPPS1          EQU  H'0001'
T1GPPS2          EQU  H'0002'
T1GPPS3          EQU  H'0003'
T1GPPS4          EQU  H'0004'


;----- T2INPPS Bits -----------------------------------------------------
T2INPPS0         EQU  H'0000'
T2INPPS1         EQU  H'0001'
T2INPPS2         EQU  H'0002'
T2INPPS3         EQU  H'0003'
T2INPPS4         EQU  H'0004'


;----- CCP1PPS Bits -----------------------------------------------------
CCP1PPS0         EQU  H'0000'
CCP1PPS1         EQU  H'0001'
CCP1PPS2         EQU  H'0002'
CCP1PPS3         EQU  H'0003'
CCP1PPS4         EQU  H'0004'


;----- CCP2PPS Bits -----------------------------------------------------
CCP2PPS0         EQU  H'0000'
CCP2PPS1         EQU  H'0001'
CCP2PPS2         EQU  H'0002'
CCP2PPS3         EQU  H'0003'
CCP2PPS4         EQU  H'0004'


;----- CWG1PPS Bits -----------------------------------------------------
CWG1PPS0         EQU  H'0000'
CWG1PPS1         EQU  H'0001'
CWG1PPS2         EQU  H'0002'
CWG1PPS3         EQU  H'0003'
CWG1PPS4         EQU  H'0004'


;----- CLCIN0PPS Bits -----------------------------------------------------
CLCIN0PPS0       EQU  H'0000'
CLCIN0PPS1       EQU  H'0001'
CLCIN0PPS2       EQU  H'0002'
CLCIN0PPS3       EQU  H'0003'
CLCIN0PPS4       EQU  H'0004'


;----- CLCIN1PPS Bits -----------------------------------------------------
CLCIN1PPS0       EQU  H'0000'
CLCIN1PPS1       EQU  H'0001'
CLCIN1PPS2       EQU  H'0002'
CLCIN1PPS3       EQU  H'0003'
CLCIN1PPS4       EQU  H'0004'


;----- CLCIN2PPS Bits -----------------------------------------------------
CLCIN2PPS0       EQU  H'0000'
CLCIN2PPS1       EQU  H'0001'
CLCIN2PPS2       EQU  H'0002'
CLCIN2PPS3       EQU  H'0003'
CLCIN2PPS4       EQU  H'0004'


;----- CLCIN3PPS Bits -----------------------------------------------------
CLCIN3PPS0       EQU  H'0000'
CLCIN3PPS1       EQU  H'0001'
CLCIN3PPS2       EQU  H'0002'
CLCIN3PPS3       EQU  H'0003'
CLCIN3PPS4       EQU  H'0004'


;----- ADCACTPPS Bits -----------------------------------------------------
ADACTPPS0        EQU  H'0000'
ADACTPPS1        EQU  H'0001'
ADACTPPS2        EQU  H'0002'
ADACTPPS3        EQU  H'0003'
ADACTPPS4        EQU  H'0004'


;----- SSP1CLKPPS Bits -----------------------------------------------------
SSP1CLKPPS0      EQU  H'0000'
SSP1CLKPPS1      EQU  H'0001'
SSP1CLKPPS2      EQU  H'0002'
SSP1CLKPPS3      EQU  H'0003'
SSP1CLKPPS4      EQU  H'0004'


;----- SSP1DATPPS Bits -----------------------------------------------------
SSP1DATPPS0      EQU  H'0000'
SSP1DATPPS1      EQU  H'0001'
SSP1DATPPS2      EQU  H'0002'
SSP1DATPPS3      EQU  H'0003'
SSP1DATPPS4      EQU  H'0004'


;----- SSP1SSPPS Bits -----------------------------------------------------
SSP1SSPPS0       EQU  H'0000'
SSP1SSPPS1       EQU  H'0001'
SSP1SSPPS2       EQU  H'0002'
SSP1SSPPS3       EQU  H'0003'
SSP1SSPPS4       EQU  H'0004'


;----- SSP2CLKPPS Bits -----------------------------------------------------
SSP2CLKPPS0      EQU  H'0000'
SSP2CLKPPS1      EQU  H'0001'
SSP2CLKPPS2      EQU  H'0002'
SSP2CLKPPS3      EQU  H'0003'
SSP2CLKPPS4      EQU  H'0004'


;----- SSP2DATPPS Bits -----------------------------------------------------
SSP2DATPPS0      EQU  H'0000'
SSP2DATPPS1      EQU  H'0001'
SSP2DATPPS2      EQU  H'0002'
SSP2DATPPS3      EQU  H'0003'
SSP2DATPPS4      EQU  H'0004'


;----- SSP2SSPPS Bits -----------------------------------------------------
SSP2SSPPS0       EQU  H'0000'
SSP2SSPPS1       EQU  H'0001'
SSP2SSPPS2       EQU  H'0002'
SSP2SSPPS3       EQU  H'0003'
SSP2SSPPS4       EQU  H'0004'


;----- RX1DTPPS Bits -----------------------------------------------------
RX1DTPPS0        EQU  H'0000'
RX1DTPPS1        EQU  H'0001'
RX1DTPPS2        EQU  H'0002'
RX1DTPPS3        EQU  H'0003'
RX1DTPPS4        EQU  H'0004'


;----- TX1CKPPS Bits -----------------------------------------------------
TX1CKPPS0        EQU  H'0000'
TX1CKPPS1        EQU  H'0001'
TX1CKPPS2        EQU  H'0002'
TX1CKPPS3        EQU  H'0003'
TX1CKPPS4        EQU  H'0004'


;----- RX2DTPPS Bits -----------------------------------------------------
RX2DTPPS0        EQU  H'0000'
RX2DTPPS1        EQU  H'0001'
RX2DTPPS2        EQU  H'0002'
RX2DTPPS3        EQU  H'0003'
RX2DTPPS4        EQU  H'0004'


;----- TX2CKPPS Bits -----------------------------------------------------
TX2CKPPS0        EQU  H'0000'
TX2CKPPS1        EQU  H'0001'
TX2CKPPS2        EQU  H'0002'
TX2CKPPS3        EQU  H'0003'
TX2CKPPS4        EQU  H'0004'


;----- RA0PPS Bits -----------------------------------------------------
RA0PPS0          EQU  H'0000'
RA0PPS1          EQU  H'0001'
RA0PPS2          EQU  H'0002'
RA0PPS3          EQU  H'0003'
RA0PPS4          EQU  H'0004'
RA0PPS5          EQU  H'0005'


;----- RA1PPS Bits -----------------------------------------------------
RA1PPS0          EQU  H'0000'
RA1PPS1          EQU  H'0001'
RA1PPS2          EQU  H'0002'
RA1PPS3          EQU  H'0003'
RA1PPS4          EQU  H'0004'
RA1PPS5          EQU  H'0005'


;----- RA2PPS Bits -----------------------------------------------------
RA2PPS0          EQU  H'0000'
RA2PPS1          EQU  H'0001'
RA2PPS2          EQU  H'0002'
RA2PPS3          EQU  H'0003'
RA2PPS4          EQU  H'0004'
RA2PPS5          EQU  H'0005'


;----- RA3PPS Bits -----------------------------------------------------
RA3PPS0          EQU  H'0000'
RA3PPS1          EQU  H'0001'
RA3PPS2          EQU  H'0002'
RA3PPS3          EQU  H'0003'
RA3PPS4          EQU  H'0004'
RA3PPS5          EQU  H'0005'


;----- RA4PPS Bits -----------------------------------------------------
RA4PPS0          EQU  H'0000'
RA4PPS1          EQU  H'0001'
RA4PPS2          EQU  H'0002'
RA4PPS3          EQU  H'0003'
RA4PPS4          EQU  H'0004'
RA4PPS5          EQU  H'0005'


;----- RA5PPS Bits -----------------------------------------------------
RA5PPS0          EQU  H'0000'
RA5PPS1          EQU  H'0001'
RA5PPS2          EQU  H'0002'
RA5PPS3          EQU  H'0003'
RA5PPS4          EQU  H'0004'
RA5PPS5          EQU  H'0005'


;----- RA6PPS Bits -----------------------------------------------------
RA6PPS0          EQU  H'0000'
RA6PPS1          EQU  H'0001'
RA6PPS2          EQU  H'0002'
RA6PPS3          EQU  H'0003'
RA6PPS4          EQU  H'0004'
RA6PPS5          EQU  H'0005'


;----- RA7PPS Bits -----------------------------------------------------
RA7PPS0          EQU  H'0000'
RA7PPS1          EQU  H'0001'
RA7PPS2          EQU  H'0002'
RA7PPS3          EQU  H'0003'
RA7PPS4          EQU  H'0004'
RA7PPS5          EQU  H'0005'


;----- RB0PPS Bits -----------------------------------------------------
RB0PPS0          EQU  H'0000'
RB0PPS1          EQU  H'0001'
RB0PPS2          EQU  H'0002'
RB0PPS3          EQU  H'0003'
RB0PPS4          EQU  H'0004'
RB0PPS5          EQU  H'0005'


;----- RB1PPS Bits -----------------------------------------------------
RB1PPS0          EQU  H'0000'
RB1PPS1          EQU  H'0001'
RB1PPS2          EQU  H'0002'
RB1PPS3          EQU  H'0003'
RB1PPS4          EQU  H'0004'
RB1PPS5          EQU  H'0005'


;----- RB2PPS Bits -----------------------------------------------------
RB2PPS0          EQU  H'0000'
RB2PPS1          EQU  H'0001'
RB2PPS2          EQU  H'0002'
RB2PPS3          EQU  H'0003'
RB2PPS4          EQU  H'0004'
RB2PPS5          EQU  H'0005'


;----- RB3PPS Bits -----------------------------------------------------
RB3PPS0          EQU  H'0000'
RB3PPS1          EQU  H'0001'
RB3PPS2          EQU  H'0002'
RB3PPS3          EQU  H'0003'
RB3PPS4          EQU  H'0004'
RB3PPS5          EQU  H'0005'


;----- RB4PPS Bits -----------------------------------------------------
RB4PPS0          EQU  H'0000'
RB4PPS1          EQU  H'0001'
RB4PPS2          EQU  H'0002'
RB4PPS3          EQU  H'0003'
RB4PPS4          EQU  H'0004'
RB4PPS5          EQU  H'0005'


;----- RB5PPS Bits -----------------------------------------------------
RB5PPS0          EQU  H'0000'
RB5PPS1          EQU  H'0001'
RB5PPS2          EQU  H'0002'
RB5PPS3          EQU  H'0003'
RB5PPS4          EQU  H'0004'
RB5PPS5          EQU  H'0005'


;----- RB6PPS Bits -----------------------------------------------------
RB6PPS0          EQU  H'0000'
RB6PPS1          EQU  H'0001'
RB6PPS2          EQU  H'0002'
RB6PPS3          EQU  H'0003'
RB6PPS4          EQU  H'0004'
RB6PPS5          EQU  H'0005'


;----- RB7PPS Bits -----------------------------------------------------
RB7PPS0          EQU  H'0000'
RB7PPS1          EQU  H'0001'
RB7PPS2          EQU  H'0002'
RB7PPS3          EQU  H'0003'
RB7PPS4          EQU  H'0004'
RB7PPS5          EQU  H'0005'


;----- RC0PPS Bits -----------------------------------------------------
RC0PPS0          EQU  H'0000'
RC0PPS1          EQU  H'0001'
RC0PPS2          EQU  H'0002'
RC0PPS3          EQU  H'0003'
RC0PPS4          EQU  H'0004'
RC0PPS5          EQU  H'0005'


;----- RC1PPS Bits -----------------------------------------------------
RC1PPS0          EQU  H'0000'
RC1PPS1          EQU  H'0001'
RC1PPS2          EQU  H'0002'
RC1PPS3          EQU  H'0003'
RC1PPS4          EQU  H'0004'
RC1PPS5          EQU  H'0005'


;----- RC2PPS Bits -----------------------------------------------------
RC2PPS0          EQU  H'0000'
RC2PPS1          EQU  H'0001'
RC2PPS2          EQU  H'0002'
RC2PPS3          EQU  H'0003'
RC2PPS4          EQU  H'0004'
RC2PPS5          EQU  H'0005'


;----- RC3PPS Bits -----------------------------------------------------
RC3PPS0          EQU  H'0000'
RC3PPS1          EQU  H'0001'
RC3PPS2          EQU  H'0002'
RC3PPS3          EQU  H'0003'
RC3PPS4          EQU  H'0004'
RC3PPS5          EQU  H'0005'


;----- RC4PPS Bits -----------------------------------------------------
RC4PPS0          EQU  H'0000'
RC4PPS1          EQU  H'0001'
RC4PPS2          EQU  H'0002'
RC4PPS3          EQU  H'0003'
RC4PPS4          EQU  H'0004'
RC4PPS5          EQU  H'0005'


;----- RC5PPS Bits -----------------------------------------------------
RC5PPS0          EQU  H'0000'
RC5PPS1          EQU  H'0001'
RC5PPS2          EQU  H'0002'
RC5PPS3          EQU  H'0003'
RC5PPS4          EQU  H'0004'
RC5PPS5          EQU  H'0005'


;----- RC6PPS Bits -----------------------------------------------------
RC6PPS0          EQU  H'0000'
RC6PPS1          EQU  H'0001'
RC6PPS2          EQU  H'0002'
RC6PPS3          EQU  H'0003'
RC6PPS4          EQU  H'0004'
RC6PPS5          EQU  H'0005'


;----- RC7PPS Bits -----------------------------------------------------
RC7PPS0          EQU  H'0000'
RC7PPS1          EQU  H'0001'
RC7PPS2          EQU  H'0002'
RC7PPS3          EQU  H'0003'
RC7PPS4          EQU  H'0004'
RC7PPS5          EQU  H'0005'


;----- ANSELA Bits -----------------------------------------------------
ANSA0            EQU  H'0000'
ANSA1            EQU  H'0001'
ANSA2            EQU  H'0002'
ANSA3            EQU  H'0003'
ANSA4            EQU  H'0004'
ANSA5            EQU  H'0005'
ANSA6            EQU  H'0006'
ANSA7            EQU  H'0007'


;----- WPUA Bits -----------------------------------------------------
WPUA0            EQU  H'0000'
WPUA1            EQU  H'0001'
WPUA2            EQU  H'0002'
WPUA3            EQU  H'0003'
WPUA4            EQU  H'0004'
WPUA5            EQU  H'0005'
WPUA6            EQU  H'0006'
WPUA7            EQU  H'0007'


;----- ODCONA Bits -----------------------------------------------------
ODCA0            EQU  H'0000'
ODCA1            EQU  H'0001'
ODCA2            EQU  H'0002'
ODCA3            EQU  H'0003'
ODCA4            EQU  H'0004'
ODCA5            EQU  H'0005'
ODCA6            EQU  H'0006'
ODCA7            EQU  H'0007'


;----- SLRCONA Bits -----------------------------------------------------
SLRA0            EQU  H'0000'
SLRA1            EQU  H'0001'
SLRA2            EQU  H'0002'
SLRA3            EQU  H'0003'
SLRA4            EQU  H'0004'
SLRA5            EQU  H'0005'
SLRA6            EQU  H'0006'
SLRA7            EQU  H'0007'


;----- INLVLA Bits -----------------------------------------------------
INLVLA0          EQU  H'0000'
INLVLA1          EQU  H'0001'
INLVLA2          EQU  H'0002'
INLVLA3          EQU  H'0003'
INLVLA4          EQU  H'0004'
INLVLA5          EQU  H'0005'
INLVLA6          EQU  H'0006'
INLVLA7          EQU  H'0007'


;----- IOCAP Bits -----------------------------------------------------
IOCAP0           EQU  H'0000'
IOCAP1           EQU  H'0001'
IOCAP2           EQU  H'0002'
IOCAP3           EQU  H'0003'
IOCAP4           EQU  H'0004'
IOCAP5           EQU  H'0005'
IOCAP6           EQU  H'0006'
IOCAP7           EQU  H'0007'


;----- IOCAN Bits -----------------------------------------------------
IOCAN0           EQU  H'0000'
IOCAN1           EQU  H'0001'
IOCAN2           EQU  H'0002'
IOCAN3           EQU  H'0003'
IOCAN4           EQU  H'0004'
IOCAN5           EQU  H'0005'
IOCAN6           EQU  H'0006'
IOCAN7           EQU  H'0007'


;----- IOCAF Bits -----------------------------------------------------
IOCAF0           EQU  H'0000'
IOCAF1           EQU  H'0001'
IOCAF2           EQU  H'0002'
IOCAF3           EQU  H'0003'
IOCAF4           EQU  H'0004'
IOCAF5           EQU  H'0005'
IOCAF6           EQU  H'0006'
IOCAF7           EQU  H'0007'


;----- ANSELB Bits -----------------------------------------------------
ANSB0            EQU  H'0000'
ANSB1            EQU  H'0001'
ANSB2            EQU  H'0002'
ANSB3            EQU  H'0003'
ANSB4            EQU  H'0004'
ANSB5            EQU  H'0005'
ANSB6            EQU  H'0006'
ANSB7            EQU  H'0007'


;----- WPUB Bits -----------------------------------------------------
WPUB0            EQU  H'0000'
WPUB1            EQU  H'0001'
WPUB2            EQU  H'0002'
WPUB3            EQU  H'0003'
WPUB4            EQU  H'0004'
WPUB5            EQU  H'0005'
WPUB6            EQU  H'0006'
WPUB7            EQU  H'0007'


;----- ODCONB Bits -----------------------------------------------------
ODCB0            EQU  H'0000'
ODCB1            EQU  H'0001'
ODCB2            EQU  H'0002'
ODCB3            EQU  H'0003'
ODCB4            EQU  H'0004'
ODCB5            EQU  H'0005'
ODCB6            EQU  H'0006'
ODCB7            EQU  H'0007'


;----- SLRCONB Bits -----------------------------------------------------
SLRB0            EQU  H'0000'
SLRB1            EQU  H'0001'
SLRB2            EQU  H'0002'
SLRB3            EQU  H'0003'
SLRB4            EQU  H'0004'
SLRB5            EQU  H'0005'
SLRB6            EQU  H'0006'
SLRB7            EQU  H'0007'


;----- INLVLB Bits -----------------------------------------------------
INLVLB0          EQU  H'0000'
INLVLB1          EQU  H'0001'
INLVLB2          EQU  H'0002'
INLVLB3          EQU  H'0003'
INLVLB4          EQU  H'0004'
INLVLB5          EQU  H'0005'
INLVLB6          EQU  H'0006'
INLVLB7          EQU  H'0007'


;----- IOCBP Bits -----------------------------------------------------
IOCBP0           EQU  H'0000'
IOCBP1           EQU  H'0001'
IOCBP2           EQU  H'0002'
IOCBP3           EQU  H'0003'
IOCBP4           EQU  H'0004'
IOCBP5           EQU  H'0005'
IOCBP6           EQU  H'0006'
IOCBP7           EQU  H'0007'


;----- IOCBN Bits -----------------------------------------------------
IOCBN0           EQU  H'0000'
IOCBN1           EQU  H'0001'
IOCBN2           EQU  H'0002'
IOCBN3           EQU  H'0003'
IOCBN4           EQU  H'0004'
IOCBN5           EQU  H'0005'
IOCBN6           EQU  H'0006'
IOCBN7           EQU  H'0007'


;----- IOCBF Bits -----------------------------------------------------
IOCBF0           EQU  H'0000'
IOCBF1           EQU  H'0001'
IOCBF2           EQU  H'0002'
IOCBF3           EQU  H'0003'
IOCBF4           EQU  H'0004'
IOCBF5           EQU  H'0005'
IOCBF6           EQU  H'0006'
IOCBF7           EQU  H'0007'


;----- ANSELC Bits -----------------------------------------------------
ANSC0            EQU  H'0000'
ANSC1            EQU  H'0001'
ANSC2            EQU  H'0002'
ANSC3            EQU  H'0003'
ANSC4            EQU  H'0004'
ANSC5            EQU  H'0005'
ANSC6            EQU  H'0006'
ANSC7            EQU  H'0007'


;----- WPUC Bits -----------------------------------------------------
WPUC0            EQU  H'0000'
WPUC1            EQU  H'0001'
WPUC2            EQU  H'0002'
WPUC3            EQU  H'0003'
WPUC4            EQU  H'0004'
WPUC5            EQU  H'0005'
WPUC6            EQU  H'0006'
WPUC7            EQU  H'0007'


;----- ODCONC Bits -----------------------------------------------------
ODCC0            EQU  H'0000'
ODCC1            EQU  H'0001'
ODCC2            EQU  H'0002'
ODCC3            EQU  H'0003'
ODCC4            EQU  H'0004'
ODCC5            EQU  H'0005'
ODCC6            EQU  H'0006'
ODCC7            EQU  H'0007'


;----- SLRCONC Bits -----------------------------------------------------
SLRC0            EQU  H'0000'
SLRC1            EQU  H'0001'
SLRC2            EQU  H'0002'
SLRC3            EQU  H'0003'
SLRC4            EQU  H'0004'
SLRC5            EQU  H'0005'
SLRC6            EQU  H'0006'
SLRC7            EQU  H'0007'


;----- INLVLC Bits -----------------------------------------------------
INLVLC0          EQU  H'0000'
INLVLC1          EQU  H'0001'
INLVLC2          EQU  H'0002'
INLVLC3          EQU  H'0003'
INLVLC4          EQU  H'0004'
INLVLC5          EQU  H'0005'
INLVLC6          EQU  H'0006'
INLVLC7          EQU  H'0007'


;----- IOCCP Bits -----------------------------------------------------
IOCCP0           EQU  H'0000'
IOCCP1           EQU  H'0001'
IOCCP2           EQU  H'0002'
IOCCP3           EQU  H'0003'
IOCCP4           EQU  H'0004'
IOCCP5           EQU  H'0005'
IOCCP6           EQU  H'0006'
IOCCP7           EQU  H'0007'


;----- IOCCN Bits -----------------------------------------------------
IOCCN0           EQU  H'0000'
IOCCN1           EQU  H'0001'
IOCCN2           EQU  H'0002'
IOCCN3           EQU  H'0003'
IOCCN4           EQU  H'0004'
IOCCN5           EQU  H'0005'
IOCCN6           EQU  H'0006'
IOCCN7           EQU  H'0007'


;----- IOCCF Bits -----------------------------------------------------
IOCCF0           EQU  H'0000'
IOCCF1           EQU  H'0001'
IOCCF2           EQU  H'0002'
IOCCF3           EQU  H'0003'
IOCCF4           EQU  H'0004'
IOCCF5           EQU  H'0005'
IOCCF6           EQU  H'0006'
IOCCF7           EQU  H'0007'


;----- WPUE Bits -----------------------------------------------------
WPUE3            EQU  H'0003'


;----- INLVLE Bits -----------------------------------------------------
INLVLE3          EQU  H'0003'


;----- IOCEP Bits -----------------------------------------------------
IOCEP3           EQU  H'0003'


;----- IOCEN Bits -----------------------------------------------------
IOCEN3           EQU  H'0003'


;----- IOCEF Bits -----------------------------------------------------
IOCEF3           EQU  H'0003'


;----- STKPTR Bits -----------------------------------------------------
STKPTR0          EQU  H'0000'
STKPTR1          EQU  H'0001'
STKPTR2          EQU  H'0002'
STKPTR3          EQU  H'0003'
STKPTR4          EQU  H'0004'


;----- TOSL Bits -----------------------------------------------------
TOSL0            EQU  H'0000'
TOSL1            EQU  H'0001'
TOSL2            EQU  H'0002'
TOSL3            EQU  H'0003'
TOSL4            EQU  H'0004'
TOSL5            EQU  H'0005'
TOSL6            EQU  H'0006'
TOSL7            EQU  H'0007'


;----- TOSH Bits -----------------------------------------------------
TOSH0            EQU  H'0000'
TOSH1            EQU  H'0001'
TOSH2            EQU  H'0002'
TOSH3            EQU  H'0003'
TOSH4            EQU  H'0004'
TOSH5            EQU  H'0005'
TOSH6            EQU  H'0006'
TOSH7            EQU  H'0007'



;==========================================================================
;
;       RAM Definitions
;
;==========================================================================
       __MAXRAM  H'1FFF'
       __BADRAM  H'000F'
       __BADRAM  H'0011'
       __BADRAM  H'0015'
       __BADRAM  H'0016'
       __BADRAM  H'0017'
       __BADRAM  H'001B'
       __BADRAM  H'001C'
       __BADRAM  H'001D'
       __BADRAM  H'001E'-H'001F'
       __BADRAM  H'008C'-H'009A'
       __BADRAM  H'010C'-H'0118'
       __BADRAM  H'0193'-H'0195'
       __BADRAM  H'019D'-H'019F'
       __BADRAM  H'0212'-H'021F'
       __BADRAM  H'0292'-H'029F'
       __BADRAM  H'0317'
       __BADRAM  H'031B'
       __BADRAM  H'031F'
       __BADRAM  H'0330'-H'036F'
       __BADRAM  H'038F'-H'039F'
       __BADRAM  H'03A0'-H'03EF'
       __BADRAM  H'040C'-H'041F'
       __BADRAM  H'0420'-H'046F'
       __BADRAM  H'048C'-H'049F'
       __BADRAM  H'04A0'-H'04EF'
       __BADRAM  H'050C'-H'051F'
       __BADRAM  H'0520'-H'056F'
       __BADRAM  H'0594'-H'059B'
       __BADRAM  H'05A0'-H'05EF'
       __BADRAM  H'0615'-H'061F'
       __BADRAM  H'0620'-H'066F'
       __BADRAM  H'068C'-H'069F'
       __BADRAM  H'06A0'-H'06EF'
       __BADRAM  H'0714'
       __BADRAM  H'0715'
       __BADRAM  H'071E'
       __BADRAM  H'071F'
       __BADRAM  H'0720'-H'076F'
       __BADRAM  H'078C'-H'0795'
       __BADRAM  H'079C'-H'079F'
       __BADRAM  H'07A0'-H'07EF'
       __BADRAM  H'0815'-H'0819'
       __BADRAM  H'0820'-H'086F'
       __BADRAM  H'0894'
       __BADRAM  H'0897'-H'089F'
       __BADRAM  H'08A0'-H'08EF'
       __BADRAM  H'090D'
       __BADRAM  H'0910'-H'091E'
       __BADRAM  H'0920'-H'096F'
       __BADRAM  H'098C'-H'098E'
       __BADRAM  H'0998'-H'099F'
       __BADRAM  H'09A0'-H'09EF'
       __BADRAM  H'0A0C'-H'0A18'
       __BADRAM  H'0A20'-H'0A6F'
       __BADRAM  H'0A8C'-H'0A9F'
       __BADRAM  H'0AA0'-H'0AEF'
       __BADRAM  H'0B0C'-H'0B1F'
       __BADRAM  H'0B20'-H'0B6F'
       __BADRAM  H'0B8C'-H'0B9F'
       __BADRAM  H'0BA0'-H'0BEF'
       __BADRAM  H'0C0C'-H'0C1F'
       __BADRAM  H'0C20'-H'0C6F'
       __BADRAM  H'0C8C'-H'0C9F'
       __BADRAM  H'0CA0'-H'0CEF'
       __BADRAM  H'0D0C'-H'0D1F'
       __BADRAM  H'0D20'-H'0D6F'
       __BADRAM  H'0D8C'-H'0D9F'
       __BADRAM  H'0DA0'-H'0DEF'
       __BADRAM  H'0E0C'-H'0E1F'
       __BADRAM  H'0E20'-H'0E6F'
       __BADRAM  H'0E8C'-H'0E9F'
       __BADRAM  H'0EA0'-H'0EEF'
       __BADRAM  H'0F0C'-H'0F1F'
       __BADRAM  H'0F20'-H'0F6F'
       __BADRAM  H'0F8C'-H'0F9F'
       __BADRAM  H'0FA0'-H'0FEF'
       __BADRAM  H'100C'-H'101F'
       __BADRAM  H'1020'-H'106F'
       __BADRAM  H'108C'-H'109F'
       __BADRAM  H'10A0'-H'10EF'
       __BADRAM  H'110C'-H'111F'
       __BADRAM  H'1120'-H'116F'
       __BADRAM  H'118C'-H'119F'
       __BADRAM  H'11A0'-H'11EF'
       __BADRAM  H'120C'-H'121F'
       __BADRAM  H'1220'-H'126F'
       __BADRAM  H'128C'-H'129F'
       __BADRAM  H'12A0'-H'12EF'
       __BADRAM  H'130C'-H'131F'
       __BADRAM  H'1320'-H'136F'
       __BADRAM  H'138C'-H'139F'
       __BADRAM  H'13A0'-H'13EF'
       __BADRAM  H'140C'-H'141F'
       __BADRAM  H'1420'-H'146F'
       __BADRAM  H'148C'-H'149F'
       __BADRAM  H'14A0'-H'14EF'
       __BADRAM  H'150C'-H'151F'
       __BADRAM  H'1520'-H'156F'
       __BADRAM  H'158C'-H'159F'
       __BADRAM  H'15A0'-H'15EF'
       __BADRAM  H'160C'-H'161F'
       __BADRAM  H'1620'-H'166F'
       __BADRAM  H'168C'-H'169F'
       __BADRAM  H'16A0'-H'16EF'
       __BADRAM  H'170C'-H'171F'
       __BADRAM  H'1720'-H'176F'
       __BADRAM  H'178C'-H'179F'
       __BADRAM  H'17A0'-H'17EF'
       __BADRAM  H'180C'-H'181F'
       __BADRAM  H'1820'-H'186F'
       __BADRAM  H'188C'-H'189F'
       __BADRAM  H'18A0'-H'18EF'
       __BADRAM  H'190C'-H'191F'
       __BADRAM  H'1920'-H'196F'
       __BADRAM  H'198C'-H'199F'
       __BADRAM  H'19A0'-H'19EF'
       __BADRAM  H'1A0C'-H'1A1F'
       __BADRAM  H'1A20'-H'1A6F'
       __BADRAM  H'1A8C'-H'1A9F'
       __BADRAM  H'1AA0'-H'1AEF'
       __BADRAM  H'1B0C'-H'1B1F'
       __BADRAM  H'1B20'-H'1B6F'
       __BADRAM  H'1B8C'-H'1B9F'
       __BADRAM  H'1BA0'-H'1BEF'
       __BADRAM  H'1C0C'-H'1C1F'
       __BADRAM  H'1C20'-H'1C6F'
       __BADRAM  H'1C8C'-H'1C9F'
       __BADRAM  H'1CA0'-H'1CEF'
       __BADRAM  H'1D0C'-H'1D1F'
       __BADRAM  H'1D20'-H'1D6F'
       __BADRAM  H'1D8C'-H'1D9F'
       __BADRAM  H'1DA0'-H'1DEF'
       __BADRAM  H'1E0C'-H'1E0E'
       __BADRAM  H'1E38'-H'1E6F'
       __BADRAM  H'1E8C'-H'1E8E'
       __BADRAM  H'1E94'-H'1E9B'
       __BADRAM  H'1E9D'-H'1EA0'
       __BADRAM  H'1EA3'-H'1EB0'
       __BADRAM  H'1EB2'-H'1EBA'
       __BADRAM  H'1EBF'-H'1EC2'
       __BADRAM  H'1EC4'
       __BADRAM  H'1ECF'-H'1EEF'
       __BADRAM  H'1F0C'-H'1F0F'
       __BADRAM  H'1F28'-H'1F37'
       __BADRAM  H'1F40'-H'1F42'
       __BADRAM  H'1F4B'-H'1F4D'
       __BADRAM  H'1F56'-H'1F64'
       __BADRAM  H'1F66'-H'1F67'
       __BADRAM  H'1F6C'-H'1F6F'
       __BADRAM  H'1F8C'-H'1FB7'
       __BADRAM  H'1FB8'-H'1FE3'
       __BADRAM  H'1FEC'

;==========================================================================
;
;       Configuration Bits
;
;   NAME            Address
;   CONFIG1           8007h
;   CONFIG2           8008h
;   CONFIG3           8009h
;   CONFIG4           800Ah
;   CONFIG5           800Bh
;
;==========================================================================

; The following is an assignment of address values for all of the
; configuration registers for the purpose of table reads
_CONFIG1        EQU  H'8007'
_CONFIG2        EQU  H'8008'
_CONFIG3        EQU  H'8009'
_CONFIG4        EQU  H'800A'
_CONFIG5        EQU  H'800B'

;----- CONFIG1 Options --------------------------------------------------
_FEXTOSC_LP          EQU  H'3FF8'; LP (crystal oscillator) optimized for 32.768kHz; PFM set to low power
_FEXTOSC_XT          EQU  H'3FF9'; XT (crystal oscillator) above 500kHz, below 4MHz; PFM set to medium power
_FEXTOSC_HS          EQU  H'3FFA'; HS (crystal oscillator) above 4MHz; PFM set to high power
_FEXTOSC_Reserved    EQU  H'3FFB'; Reserved
_FEXTOSC_OFF         EQU  H'3FFC'; Oscillator not enabled
_FEXTOSC_ECL         EQU  H'3FFD'; EC below 500kHz; PFM set to low power
_FEXTOSC_ECM         EQU  H'3FFE'; EC for 500kHz to 8MHz; PFM set to medium power
_FEXTOSC_ECH         EQU  H'3FFF'; EC above 8MHz; PFM set to high power

_RSTOSC_HFINT32      EQU  H'3F8F'; HFINTOSC with OSCFRQ= 32 MHz and CDIV = 1:1
_RSTOSC_HFINTPLL     EQU  H'3F9F'; HFINTOSC with 2x PLL, with OSCFRQ = 16 MHz and CDIV = 1:1 (FOSC = 32 MHz)
_RSTOSC_EXT4X        EQU  H'3FAF'; EXTOSC with 4x PLL, with EXTOSC operating per FEXTOSC bits
_RSTOSC_Reserved     EQU  H'3FBF'; Reserved
_RSTOSC_SOSC         EQU  H'3FCF'; SOSC
_RSTOSC_LFINT        EQU  H'3FDF'; LFINTOSC
_RSTOSC_HFINT1       EQU  H'3FEF'; HFINTOSC (1MHz)
_RSTOSC_EXT1X        EQU  H'3FFF'; EXTOSC operating per FEXTOSC bits

_CLKOUTEN_ON         EQU  H'3EFF'; CLKOUT function is enabled; FOSC/4 clock appears at OSC2
_CLKOUTEN_OFF        EQU  H'3FFF'; CLKOUT function is disabled; i/o or oscillator function on OSC2

_CSWEN_OFF           EQU  H'37FF'; The NOSC and NDIV bits cannot be changed by user software
_CSWEN_ON            EQU  H'3FFF'; Writing to NOSC and NDIV is allowed

_FCMEN_OFF           EQU  H'1FFF'; FSCM timer disabled
_FCMEN_ON            EQU  H'3FFF'; FSCM timer enabled

;----- CONFIG2 Options --------------------------------------------------
_MCLRE_OFF           EQU  H'3FFE'; MCLR pin function is port defined function
_MCLRE_ON            EQU  H'3FFF'; MCLR pin is Master Clear function

_PWRTE_ON            EQU  H'3FFD'; PWRT enabled
_PWRTE_OFF           EQU  H'3FFF'; PWRT disabled

_LPBOREN_ON          EQU  H'3FDF'; ULPBOR enabled
_LPBOREN_OFF         EQU  H'3FFF'; ULPBOR disabled

_BOREN_OFF           EQU  H'3F3F'; Brown-out reset disabled
_BOREN_SBOREN        EQU  H'3F7F'; Brown-out reset enabled according to SBOREN bit
_BOREN_NSLEEP        EQU  H'3FBF'; Brown-out Reset enabled while running, disabled in sleep; SBOREN is ignored
_BOREN_ON            EQU  H'3FFF'; Brown-out Reset Enabled, SBOREN bit is ignored

_BORV_HI             EQU  H'3DFF'; Brown-out Reset Voltage (VBOR) is set to 2.7V
_BORV_LO             EQU  H'3FFF'; Brown-out Reset Voltage (VBOR) set to 1.9V on LF, and 2.45V on F Devices

_ZCD_ON              EQU  H'3BFF'; Zero-cross detect circuit is always enabled
_ZCD_OFF             EQU  H'3FFF'; Zero-cross detect circuit is disabled at POR.

_PPS1WAY_OFF         EQU  H'37FF'; The PPSLOCK bit can be set and cleared repeatedly by software
_PPS1WAY_ON          EQU  H'3FFF'; The PPSLOCK bit can be cleared and set only once in software

_STVREN_OFF          EQU  H'2FFF'; Stack Overflow or Underflow will not cause a reset
_STVREN_ON           EQU  H'3FFF'; Stack Overflow or Underflow will cause a reset

;----- CONFIG3 Options --------------------------------------------------
_WDTCPS_WDTCPS_0     EQU  H'3FE0'; Divider ratio 1:32
_WDTCPS_WDTCPS_1     EQU  H'3FE1'; Divider ratio 1:64
_WDTCPS_WDTCPS_2     EQU  H'3FE2'; Divider ratio 1:128
_WDTCPS_WDTCPS_3     EQU  H'3FE3'; Divider ratio 1:256
_WDTCPS_WDTCPS_4     EQU  H'3FE4'; Divider ratio 1:512
_WDTCPS_WDTCPS_5     EQU  H'3FE5'; Divider ratio 1:1024
_WDTCPS_WDTCPS_6     EQU  H'3FE6'; Divider ratio 1:2048
_WDTCPS_WDTCPS_7     EQU  H'3FE7'; Divider ratio 1:4096
_WDTCPS_WDTCPS_8     EQU  H'3FE8'; Divider ratio 1:8192
_WDTCPS_WDTCPS_9     EQU  H'3FE9'; Divider ratio 1:16384
_WDTCPS_WDTCPS_10    EQU  H'3FEA'; Divider ratio 1:32768
_WDTCPS_WDTCPS_11    EQU  H'3FEB'; Divider ratio 1:65536
_WDTCPS_WDTCPS_12    EQU  H'3FEC'; Divider ratio 1:131072
_WDTCPS_WDTCPS_13    EQU  H'3FED'; Divider ratio 1:262144
_WDTCPS_WDTCPS_14    EQU  H'3FEE'; Divider ratio 1:524299
_WDTCPS_WDTCPS_15    EQU  H'3FEF'; Divider ratio 1:1048576
_WDTCPS_WDTCPS_16    EQU  H'3FF0'; Divider ratio 1:2097152
_WDTCPS_WDTCPS_17    EQU  H'3FF1'; Divider ratio 1:4194304
_WDTCPS_WDTCPS_18    EQU  H'3FF2'; Divider ratio 1:8388608
_WDTCPS_WDTCPS_19    EQU  H'3FF3'; Divider ratio 1:32
_WDTCPS_WDTCPS_20    EQU  H'3FF4'; Divider ratio 1:32
_WDTCPS_WDTCPS_21    EQU  H'3FF5'; Divider ratio 1:32
_WDTCPS_WDTCPS_22    EQU  H'3FF6'; Divider ratio 1:32
_WDTCPS_WDTCPS_23    EQU  H'3FF7'; Divider ratio 1:32
_WDTCPS_WDTCPS_24    EQU  H'3FF8'; Divider ratio 1:32
_WDTCPS_WDTCPS_25    EQU  H'3FF9'; Divider ratio 1:32
_WDTCPS_WDTCPS_26    EQU  H'3FFA'; Divider ratio 1:32
_WDTCPS_WDTCPS_27    EQU  H'3FFB'; Divider ratio 1:32
_WDTCPS_WDTCPS_28    EQU  H'3FFC'; Divider ratio 1:32
_WDTCPS_WDTCPS_29    EQU  H'3FFD'; Divider ratio 1:32
_WDTCPS_WDTCPS_30    EQU  H'3FFE'; Divider ratio 1:32
_WDTCPS_WDTCPS_31    EQU  H'3FFF'; Divider ratio 1:65536; software control of WDTPS

_WDTE_OFF            EQU  H'3F9F'; WDT Disabled, SWDTEN is ignored
_WDTE_SWDTEN         EQU  H'3FBF'; WDT enabled/disabled by SWDTEN bit in WDTCON0
_WDTE_NSLEEP         EQU  H'3FDF'; WDT enabled while sleep=0, suspended when sleep=1; SWDTEN ignored
_WDTE_ON             EQU  H'3FFF'; WDT enabled regardless of sleep; SWDTEN ignored

_WDTCWS_WDTCWS_0     EQU  H'38FF'; window delay = 87.5 percent of time; no software control; keyed access required
_WDTCWS_WDTCWS_1     EQU  H'39FF'; window delay = 75 percent of time; no software control; keyed access required
_WDTCWS_WDTCWS_2     EQU  H'3AFF'; window delay = 62.5 percent of time; no software control; keyed access required
_WDTCWS_WDTCWS_3     EQU  H'3BFF'; window delay = 50 percent of time; no software control; keyed access required
_WDTCWS_WDTCWS_4     EQU  H'3CFF'; window delay = 37.5 percent of time; no software control; keyed access required
_WDTCWS_WDTCWS_5     EQU  H'3DFF'; window delay = 25 percent of time; no software control; keyed access required
_WDTCWS_WDTCWS_6     EQU  H'3EFF'; window always open (100%); no software control; keyed access required
_WDTCWS_WDTCWS_7     EQU  H'3FFF'; window always open (100%); software control; keyed access not required

_WDTCCS_LFINTOSC     EQU  H'07FF'; WDT reference clock is the 31.0kHz LFINTOSC output
_WDTCCS_HFINTOSC     EQU  H'0FFF'; WDT reference clock is the 31.25 kHz HFINTOSC
_WDTCCS_SOSC         EQU  H'17FF'; WDT reference clock is the 32kHz secondary oscillator
_WDTCCS_SC           EQU  H'3FFF'; Software Control

;----- CONFIG4 Options --------------------------------------------------
_BBSIZE_BB64K        EQU  H'3FF8'; * half of user program memory
_BBSIZE_BB32K        EQU  H'3FF9'; * half of user program memory
_BBSIZE_BB16K        EQU  H'3FFA'; * half of user program memory
_BBSIZE_BB8K         EQU  H'3FFB'; * half of user program memory
_BBSIZE_BB4K         EQU  H'3FFC'; * half of user program memory
_BBSIZE_BB2K         EQU  H'3FFD'; 2048 byte boot block size
_BBSIZE_BB1K         EQU  H'3FFE'; 1024 byte boot block size
_BBSIZE_BB512        EQU  H'3FFF'; 512 byte boot block size

_BBEN_ON             EQU  H'3FF7'; Boot Block enabled
_BBEN_OFF            EQU  H'3FFF'; Boot Block disabled

_SAFEN_ON            EQU  H'3FEF'; SAF enabled
_SAFEN_OFF           EQU  H'3FFF'; SAF disabled

_WRTAPP_ON           EQU  H'3F7F'; Application Block write protected
_WRTAPP_OFF          EQU  H'3FFF'; Application Block not write protected

_WRTB_ON             EQU  H'3EFF'; Boot Block write protected
_WRTB_OFF            EQU  H'3FFF'; Boot Block not write protected

_WRTC_ON             EQU  H'3DFF'; Configuration Register write protected
_WRTC_OFF            EQU  H'3FFF'; Configuration Register not write protected

_WRTD_ON             EQU  H'3BFF'; Data EEPROM write protected
_WRTD_OFF            EQU  H'3FFF'; Data EEPROM not write protected

_WRTSAF_ON           EQU  H'37FF'; SAF write protected
_WRTSAF_OFF          EQU  H'3FFF'; SAF not write protected

_LVP_OFF             EQU  H'1FFF'; High Voltage on MCLR/Vpp must be used for programming
_LVP_ON              EQU  H'3FFF'; Low Voltage programming enabled. MCLR/Vpp pin function is MCLR.

;----- CONFIG5 Options --------------------------------------------------
_CP_ON               EQU  H'3FFE'; UserNVM code protection enabled
_CP_OFF              EQU  H'3FFF'; UserNVM code protection disabled

;----- DEVID Equates --------------------------------------------------
_DEVID1          EQU  H'8006'

;----- IDLOC Equates --------------------------------------------------
_IDLOC0          EQU  H'8000'
_IDLOC1          EQU  H'8001'
_IDLOC2          EQU  H'8002'
_IDLOC3          EQU  H'8003'

        LIST
