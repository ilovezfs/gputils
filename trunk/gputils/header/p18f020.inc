        LIST
; P18F020.INC  Standard Header File, Version 0.10   Microchip Technology, Inc.
        NOLIST

; This header file defines configurations, registers, and other useful bits of
; information for the PIC18F020 microcontroller.  These names are taken to match 
; the data sheets as closely as possible.  

; Note that the processor must be selected before this file is 
; included.  The processor may be selected the following ways:

;       1. Command line switch:
;               C:\ MPASM MYFILE.ASM /PIC18F020
;       2. LIST directive in the source file
;               LIST   P=PIC18F020
;       3. Processor Type entry in the MPASM full-screen interface
;       4. Setting the processor in the MPLAB Project Dialog

;==========================================================================
;
;       Revision History
;
;==========================================================================
;Rev:   Date:        Details:                                         Who:
;0.90  15 May 2001   Preliminary Release                               dzb
;0.91  13 Jun 2001   Change RAM map and config bits                    MG

;==========================================================================
;
;       Verify Processor
;
;==========================================================================

        IFNDEF __18F020
           MESSG "Processor-header file mismatch.  Verify selected processor."
        ENDIF

;==========================================================================
;       18Fxxx Family        EQUates
;==========================================================================

FSR0            EQU 0
FSR1            EQU 1
FSR2            EQU 2

FAST            EQU 1

W               EQU 0
A               EQU 0
ACCESS          EQU 0
BANKED		EQU 1

;==========================================================================
;       16Cxxx/17Cxxx Substitutions
;==========================================================================

  #define clrw clrf WREG       ; PIC16Cxxx code substitution (WREG is addressable)
  #define CLRW CLRF WREG       ; PIC16Cxxx code substitution (WREG is addressable)
  #define negw negf WREG       ; PIC16Cxxx code substitution (WREG is addressable)
  #define NEGW NEGF WREG       ; PIC16Cxxx code substitution (WREG is addressable)
  #define movpf movff          ; PIC17Cxxx code substitution
  #define MOVPF MOVFF          ; PIC17Cxxx code substitution
  #define movfp movff          ; PIC17Cxxx code substitution
  #define MOVFP MOVFF          ; PIC17Cxxx code substitution
  #define lcall call           ; PIC17Cxxx code substitution
  #define LCALL CALL           ; PIC17Cxxx code substitution
  #define lgoto goto           ; PIC17Cxxx code substitution
  #define LGOTO GOTO           ; PIC17Cxxx code substitution
  #define DDRA  TRISA          ; PIC17Cxxx SFR substitution
  #define DDRB  TRISB          ; PIC17Cxxx SFR substitution
  #define DDRC  TRISC          ; PIC17Cxxx SFR substitution
  #define DDRD  TRISD          ; PIC17Cxxx SFR substitution
  #define DDRE  TRISE          ; PIC17Cxxx SFR substitution

;==========================================================================
;
;       Register Definitions
;
;==========================================================================

;----- Register Files -----------------------------------------------------
TOSH            EQU  H'0FFE'
TOSL            EQU  H'0FFD'
STKPTR          EQU  H'0FFC'
PCLATU          EQU  H'0FFB'
PCLATH          EQU  H'0FFA'
PCL             EQU  H'0FF9'
TBLPTRU         EQU  H'0FF8'
TBLPTRH         EQU  H'0FF7'
TBLPTRL         EQU  H'0FF6'
TABLAT          EQU  H'0FF5'
PRODH           EQU  H'0FF4'
PRODL           EQU  H'0FF3'

INTCON          EQU  H'0FF2'
INTCON1         EQU  H'0FF2'
INTCON2         EQU  H'0FF1'

INDF0           EQU  H'0FEF'
POSTINC0        EQU  H'0FEE'
POSTDEC0        EQU  H'0FED'
PREINC0         EQU  H'0FEC'
PLUSW0          EQU  H'0FEB'
FSR0H           EQU  H'0FEA'
FSR0L           EQU  H'0FE9'
WREG            EQU  H'0FE8'

INDF1           EQU  H'0FE7'
POSTINC1        EQU  H'0FE6'
POSTDEC1        EQU  H'0FE5'
PREINC1         EQU  H'0FE4'
PLUSW1          EQU  H'0FE3'
FSR1H           EQU  H'0FE2'
FSR1L           EQU  H'0FE1'
BSR             EQU  H'0FE0'

INDF2           EQU  H'0FDF'
POSTINC2        EQU  H'0FDE'
POSTDEC2        EQU  H'0FDD'
PREINC2         EQU  H'0FDC'
PLUSW2          EQU  H'0FDB'
FSR2H           EQU  H'0FDA'
FSR2L           EQU  H'0FD9'
STATUS          EQU  H'0FD8'

TMR0H           EQU  H'0FD7'
TMR0L           EQU  H'0FD6'
T0CON           EQU  H'0FD5'

OSCCON          EQU  H'0FD3'
LVDCON          EQU  H'0FD2'
WDTCON          EQU  H'0FD1'
RCON            EQU  H'0FD0'

EEADR           EQU  H'0FA9'
EEDATA          EQU  H'0FA8'
EECON2          EQU  H'0FA7'
EECON1          EQU  H'0FA6'

IPR2            EQU  H'0FA2'
PIR2            EQU  H'0FA1'
PIE2            EQU  H'0FA0'

OSCTUNE         EQU  H'0F9B'

TRISB           EQU  H'0F93'
LATB            EQU  H'0F8A'
PORTB           EQU  H'0F81'

WPUB            EQU  H'0F79'
IOCB            EQU  H'0F78'

;----- STKPTR Bits --------------------------------------------------------
STKFUL          EQU  H'0007'
STKUNF          EQU  H'0006'

;----- INTCON Bits --------------------------------------------------------
GIE             EQU  H'0007'
GIEH            EQU  H'0007'
PEIE            EQU  H'0006'
GIEL            EQU  H'0006'
TMR0IE          EQU  H'0005'
T0IE            EQU  H'0005'      ; For backward compatibility
INT0IE          EQU  H'0004'
INT0E           EQU  H'0004'      ; For backward compatibility
RBIE            EQU  H'0003'
TMR0IF          EQU  H'0002'
T0IF            EQU  H'0002'      ; For backward compatibility
INT0IF          EQU  H'0001'
INT0F           EQU  H'0001'      ; For backward compatibility
RBIF            EQU  H'0000'

;----- INTCON2 Bits --------------------------------------------------------
NOT_RBPU        EQU  H'0007'
RBPU            EQU  H'0007'
INTEDG0         EQU  H'0006'
TMR0IP          EQU  H'0002'
T0IP            EQU  H'0002'      ; For compatibility with T0IE and T0IF
RBIP            EQU  H'0000'

;----- STATUS Bits --------------------------------------------------------
N               EQU  H'0004'
OV              EQU  H'0003'
Z               EQU  H'0002'
DC              EQU  H'0001'
C               EQU  H'0000'

;----- T0CON Bits ---------------------------------------------------------
TMR0ON          EQU  H'0007'
T08BIT           EQU  H'0006'
T0CS            EQU  H'0005'
T0SE            EQU  H'0004'
T0PS3           EQU  H'0003'
T0PS2           EQU  H'0002'
T0PS1           EQU  H'0001'
T0PS0           EQU  H'0000'

;----- OSCCON Bits --------------------------------------------------------
IRCF2           EQU  H'0006'
IRCF1           EQU  H'0005'
IRCF0           EQU  H'0004'
OSTO            EQU  H'0003'
IESO            EQU  H'0002'
SCS             EQU  H'0000'

;----- LVDCON Bits ---------------------------------------------------------
BGST            EQU  H'0005'
LVDEN           EQU  H'0004'
LVV3            EQU  H'0003'
LVV2            EQU  H'0002'
LVV1            EQU  H'0001'
LVV0            EQU  H'0000'

;----- WDTCON Bits ---------------------------------------------------------
SWP2            EQU  H'0003'
SWP1            EQU  H'0002'
SWP0            EQU  H'0001'
SWDTEN          EQU  H'0000'

;----- RCON Bits -----------------------------------------------------------
IPEN            EQU  H'0007'
NOT_RI          EQU  H'0004'
RI              EQU  H'0004'
NOT_TO          EQU  H'0003'
TO              EQU  H'0003'
NOT_PD          EQU  H'0002'
PD              EQU  H'0002'
NOT_POR         EQU  H'0001'
POR             EQU  H'0001'
NOT_BOR         EQU  H'0000'
BOR             EQU  H'0000'

;----- EECON1 Bits --------------------------------------------------------
EEPGD           EQU  H'0007'
FREE            EQU  H'0004'
WRERR           EQU  H'0003'
WREN            EQU  H'0002'
WR              EQU  H'0001'
RD              EQU  H'0000'

;----- IPR2 Bits ----------------------------------------------------------
EEIP            EQU  H'0004'
LVDIP           EQU  H'0002'

;----- PIR2 Bits ----------------------------------------------------------
EEIF            EQU  H'0004'
LVDIF           EQU  H'0002'

;----- PIE2 Bits ----------------------------------------------------------
EEIE            EQU  H'0004'
LVDIE           EQU  H'0002'

;==========================================================================
;
;       I/O Pin Name Definitions
;
;==========================================================================

;----- PORTB ------------------------------------------------------------------
RB0             EQU  0
INT0            EQU  0
RB1             EQU  1
RB2             EQU  2
RB3             EQU  3
RB4             EQU  4
RB5             EQU  5

;==========================================================================
;
;       RAM Definition
;
;==========================================================================

	__MAXRAM H'FFF'
	__BADRAM H'100'-H'F77', H'F7A'-H'F80', H'F82'-H'F89'
	__BADRAM H'F8B'-H'F92', H'F94'-H'F9A', H'F9C'-H'F9F'
	__BADRAM H'FA3'-H'FA5', H'FAB'-H'FCF', H'FD4', H'FFF'

;==========================================================================
;
;       Configuration Bits
;
;     Data Sheet    Include File                  Address
;     CONFIG1L    = Configuration Byte 1L         300000h
;     CONFIG1H    = Configuration Byte 1H         300001h
;     CONFIG2L    = Configuration Byte 2L         300002h
;     CONFIG2H    = Configuration Byte 2H         300003h
;
;==========================================================================
;
;Configuration Byte 1L Options (300000h)
_CP_ALL_1L        EQU  H'EE'	; Code Protect all blocks (0-FFFh)
_CP1_1L           EQU  H'EF'	; Code protect block 1 (400-FFFh)
_CP0_1L           EQU  H'FE'    ; code Protect block 0 (0-3FFh)
_CP_OFF_1L        EQU  H'FF'    ; Not code protected

_DP_ON_1L         EQU  H'F7'
_DP_OFF_1L        EQU  H'FF'

_EBTR0_ON_1L      EQU  H'BB'    ; External Table Read protection block 0 (000-3FFh)
_EBTR0_OFF_1L     EQU  H'FF'    ; External table reads to block 0 allowed
_EBTR1_ON_1L      EQU  H'BF'	; External Table Read protection block 1 (400-FFFh)
_EBTR1_OFF_1L     EQU  H'FB'	; External Table Read to block 1 allowed
_EBTR_ALL_1L      EQU  H'FB'	; External Table Read protection all blocks (000-FFFh)

_WRT0_ON_1L       EQU  H'DF'	; Table Write protection block 0 (000-3FFh)
_WRT0_OFF_1L      EQU  H'FF'	; Block 0 not protected
_WRT1_ON_1L       EQU  H'FD'    ; Table Write protection block 1 (400-FFFh)
_WRT1_OFF_1L      EQU  H'FF'	; Block 1 not protected
_WRT_ALL_1L       EQU  H'DD'    ; Table Write protection all blocks (000-FFFh)

;Configuration Byte 1H Options (300001h)
_OSCS_ON_1H       EQU  H'DF'	; Oscillator Switch enable
_OSCS_OFF_1H      EQU  H'FF'

_MCLRE_ON_1H      EQU  H'EF'
_MCLRE_OFF_1H     EQU  H'FF'

_LP_OSC_1H        EQU  H'F8'	; Oscillator type
_XT_OSC_1H        EQU  H'F9'
_HS_OSC_1H        EQU  H'FA'
_RC_OSC_1H        EQU  H'FB'	; External RC with RB4 function on RB4/OSC2/CLKOUT
_INT_OSC_1H       EQU  H'FC'	; Internal RC with RB4 function on RB4/OSC2/CLKOUT
_INTIO_OSC_1H     EQU  H'FD'	; Internal RC with CLOCKOUT function on RB4/OSC2/CLKOUT
_ECIO_1H          EQU  H'FE'	; External Clock w/OSC2 output divide by 4
_RCIO_OSC_1H      EQU  H'FF'	; External RC Clockout on RB4/OSC2/CLKOUT

;Configuration Byte 2L Options (300002h)
_BOR_ON_2L        EQU  H'FF'	; Brown-Out Reset enable
_BOR_OFF_2L       EQU  H'FD'
_PWRT_OFF_2L      EQU  H'FF'	; Power-Up Timer enable
_PWRT_ON_2L       EQU  H'FE'

;Configuration Byte 2H Options (300003h)
_WDT_ON_2H        EQU  H'FF'	; Watch Dog Timer enable
_WDT_OFF_2H       EQU  H'FE'
_WDTPS_128_2H     EQU  H'FF'	; Watch Dog Timer PostScaler count
_WDTPS_64_2H      EQU  H'FD'
_WDTPS_32_2H      EQU  H'FB'
_WDTPS_16_2H      EQU  H'F9'
_WDTPS_8_2H       EQU  H'F7'
_WDTPS_4_2H       EQU  H'F5'
_WDTPS_2_2H       EQU  H'F3'
_WDTPS_1_2H       EQU  H'F1'
_WDTLE_ON_2H      EQU  H'EF'	; Watch Dog Timer Long Delay enable
_WDTLE_OFF_2H     EQU  H'FF'
_STVRE_ON_2H      EQU  H'FF'
_STVRE_OFF_2H     EQU  H'DF'

; To use the Configuration Bits, place the following lines in your source code
;  in the following format, and change the configuration value to the desired 
;  setting (such as _CP_ALL_1L).  These are currently commented out here
;  and each __CONFIG line should have the preceding semicolon removed when
;  pasted into your source code.

;Program Configuration Register 1L
;		__CONFIG    _CONFIG1L, _CP_OFF_0

;Program Configuration Register 1H
;		__CONFIG    _CONFIG1H, _OSCS_OFF_1 & _RCIO_OSC_1

;Program Configuration Register 2L
;		__CONFIG    _CONFIG2L, _BOR_ON_2 & _BORV_25_2 & _PWRT_OFF_2

;Program Configuration Register 2L
;		__CONFIG    _CONFIG2H, _WDT_ON_3 & _WDTPS_128_3

;ID Locations Register 0
;		__IDLOCS    _IDLOC0, <expression>

;ID Locations Register 1
;		__IDLOCS    _IDLOC1, <expression>

;ID Locations Register 2
;		__IDLOCS    _IDLOC2, <expression>

;ID Locations Register 3
;		__IDLOCS    _IDLOC3, <expression>

;ID Locations Register 4
;		__IDLOCS    _IDLOC4, <expression>

;ID Locations Register 5
;		__IDLOCS    _IDLOC5, <expression>

;ID Locations Register 6
;		__IDLOCS    _IDLOC6, <expression>

;ID Locations Register 7
;		__IDLOCS    _IDLOC7, <expression>

;Device ID registers hold device ID and revision number and can only be read
;Device ID Register 1
;               DEV2, DEV1, DEV0, REV4, REV3, REV2, REV1, REV0
;Device ID Register 2
;               DEV10, DEV9, DEV8, DEV7, DEV6, DEV5, DEV4, DEV3

;  The following is a assignment of address values for all of the configuration
;  registers for the purpose of table reads
_CONFIG1L        EQU    H'300000'
_CONFIG1H        EQU    H'300001'
_CONFIG2L        EQU    H'300002'
_CONFIG2H        EQU    H'300003'
_FOSCCAL         EQU    H'300104'
_DEVID1          EQU    H'3FFFFE'
_DEVID2          EQU    H'3FFFFF'
_IDLOC0          EQU    H'200000'
_IDLOC1          EQU    H'200001'
_IDLOC2          EQU    H'200002'
_IDLOC3          EQU    H'200003'
_IDLOC4          EQU    H'200004'
_IDLOC5          EQU    H'200005'
_IDLOC6          EQU    H'200006'
_IDLOC7          EQU    H'200007'

;==========================================================================
        LIST
