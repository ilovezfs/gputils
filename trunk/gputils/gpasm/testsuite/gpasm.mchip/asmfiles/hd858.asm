; HD658.ASM

; This file tests header file usage for the specified processor.

        LIST    P=18C858
;;;; Begin: Changed in gputils
;;;;        INCLUDE "p18c858.inc"
        INCLUDE "P18C858.INC"
;;;; End: Changed in gputils

        DATA TOSU,  H'0FFF'
        DATA TOSH,  H'0FFE'
        DATA TOSL,  H'0FFD'
        DATA STKPTR,  H'0FFC'
        DATA PCLATU,  H'0FFB'
        DATA PCLATH,  H'0FFA'
        DATA PCL,     H'0FF9'
        DATA TBLPTRU,  H'0FF8'
        DATA TBLPTRH,  H'0FF7'
        DATA TBLPTRL,  H'0FF6'
        DATA TABLAT,   H'0FF5'
        DATA PRODH,    H'0FF4'
        DATA PRODL,    H'0FF3'

        DATA INTCON,    H'0FF2'
        DATA INTCON1,   H'0FF2'
        DATA INTCON2,   H'0FF1'
        DATA INTCON3,   H'0FF0'

        DATA INDF0,     H'0FEF'
        DATA POSTINC0,  H'0FEE'
        DATA POSTDEC0,  H'0FED'
        DATA PREINC0,   H'0FEC'
        DATA PLUSW0,    H'0FEB'
        DATA FSR0H,     H'0FEA'
        DATA FSR0L,     H'0FE9'
        DATA WREG,      H'0FE8'

        DATA INDF1,      H'0FE7'
        DATA POSTINC1,   H'0FE6'
        DATA POSTDEC1,   H'0FE5'
        DATA PREINC1,    H'0FE4'
        DATA PLUSW1,     H'0FE3'
        DATA FSR1H,      H'0FE2'
        DATA FSR1L,      H'0FE1'
        DATA BSR,        H'0FE0'

        DATA INDF2,      H'0FDF'
        DATA POSTINC2,   H'0FDE'
        DATA POSTDEC2,   H'0FDD'
        DATA PREINC2,    H'0FDC'
        DATA PLUSW2,     H'0FDB'
        DATA FSR2L,      H'0FD9'
        DATA STATUS,     H'0FD8'

        DATA TMR0H,      H'0FD7'
        DATA TMR0L,      H'0FD6'
        DATA T0CON,      H'0FD5'

        DATA OSCCON,    H'0FD3'
        DATA LVDCON,    H'0FD2'
        DATA WDTCON,    H'0FD1'
        DATA RCON,      H'0FD0'

        DATA TMR1H,     H'0FCF'
        DATA TMR1L,     H'0FCE'
        DATA T1CON,     H'0FCD'
        DATA TMR2,      H'0FCC'
        DATA PR2,       H'0FCB'
        DATA T2CON,     H'0FCA'

        DATA SSPBUF,    H'0FC9'
        DATA SSPADD,    H'0FC8'
        DATA SSPSTAT,   H'0FC7'
        DATA SSPCON1,   H'0FC6'
        DATA SSPCON2,   H'0FC5'

        DATA ADRESH,    H'0FC4'
        DATA ADRESL,    H'0FC3'
        DATA ADCON0,    H'0FC2'
        DATA ADCON1,    H'0FC1'


        DATA CCPR1H,    H'0FBF'
        DATA CCPR1L,    H'0FBE'
        DATA CCPR2H,    H'0FBC'
        DATA CCPR2L,    H'0FBB'
        DATA CCP2CON,   H'0FBA'


        DATA TMR3H,     H'0FB3'
        DATA TMR3L,     H'0FB2'
        DATA T3CON,     H'0FB1'

        DATA SPBRG,     H'0FAF'
        DATA RCREG,     H'0FAE'
        DATA TXREG,     H'0FAD'
        DATA TXSTA,     H'0FAC'
        DATA RCSTA,     H'0FAB'


        DATA IPR2,      H'0FA2'
        DATA PIR2,      H'0FA1'
        DATA PIE2,      H'0FA0'
        DATA IPR1,      H'0F9F'
        DATA PIR1,      H'0F9E'
        DATA PIE1,      H'0F9D'

        DATA DDRK,      H'0F7F'
        DATA TRISJ,     H'0F9A'
        DATA TRISH,     H'0F99'
        DATA TRISG,     H'0F98'
        DATA TRISF,     H'0F97'
        DATA TRISE,     H'0F96'
        DATA TRISD,     H'0F95'
        DATA TRISC,     H'0F94'
        DATA TRISB,     H'0F93'
        DATA TRISA,     H'0F92'

        DATA LATK,      H'0F7E'
        DATA LATJ,      H'0F91'
        DATA LATH,      H'0F90'
        DATA LATG,      H'0F8F'
        DATA LATF,      H'0F8E'
        DATA LATE,      H'0F8D'
        DATA LATD,      H'0F8C'
        DATA LATC,      H'0F8B'
        DATA LATB,      H'0F8A'
        DATA LATA,      H'0F89'

        DATA PORTK,     H'0F7D'
        DATA PORTJ,     H'0F88'
        DATA PORTH,     H'0F87'
        DATA PORTG,     H'0F86'
        DATA PORTF,     H'0F85'
        DATA PORTE,     H'0F84'
        DATA PORTD,     H'0F83'
        DATA PORTC,     H'0F82'
        DATA PORTB,     H'0F81'
        DATA PORTA,     H'0F80'

        DATA TXERRCNT,  H'0F76'
        DATA RXERRCNT,  H'0F75'
        DATA COMSTAT,   H'0F74'

        DATA CIOCON,    H'0F73'

        DATA BRGCON3,   H'0F72'
        DATA BRGCON2,   H'0F71'
        DATA BRGCON1,   H'0F70'

        DATA CANCON,    H'0F6F'
        DATA CANSTAT,   H'0F6E'
 
        DATA RXB0D7,    H'0F6D'
        DATA RXB0D6,    H'0F6C'
        DATA RXB0D5,    H'0F6B'
        DATA RXB0D4,    H'0F6A'
        DATA RXB0D3,    H'0F69'
        DATA RXB0D2,    H'0F68'
        DATA RXB0D1,    H'0F67'
        DATA RXB0D0,    H'0F66'

        DATA RXB0DLC,   H'0F65'

        DATA RXB0EIDL,  H'0F64'
        DATA RXB0EIDH,  H'0F63'
        DATA RXB0SIDL,  H'0F62'
        DATA RXB0SIDH,  H'0F61'
        DATA RXB0CON,   H'0F60'


        DATA CANSTAT_RO1, H'0F5E'     ;CANSTAT is repeated

        DATA RXB1D7,    H'0F5D'
        DATA RXB1D6,    H'0F5C'
        DATA RXB1D4,    H'0F5A'
        DATA RXB1D3,    H'0F59'
        DATA RXB1D2,    H'0F58'
        DATA RXB1D1,    H'0F57'
        DATA RXB1D0,    H'0F56'

        DATA RXB1DLC,   H'0F55'

        DATA RXB1EIDL,  H'0F54'
        DATA RXB1EIDH,  H'0F53'
        DATA RXB1SIDL,  H'0F52'
        DATA RXB1SIDH,  H'0F51'
        DATA RXB1CON,   H'0F50'


        DATA CANSTAT_RO2, H'0F4E'     ;CANSTAT is repeated

        DATA TXB0D7,    H'0F4D'
        DATA TXB0D6,    H'0F4C'
        DATA TXB0D5,    H'0F4B'
        DATA TXB0D4,    H'0F4A'
        DATA TXB0D3,    H'0F49'
        DATA TXB0D2,    H'0F48'
        DATA TXB0D0,    H'0F46'

        DATA TXB0DLC,   H'0F45'

        DATA TXB0EIDL,  H'0F44'
        DATA TXB0EIDH,  H'0F43'
        DATA TXB0SIDL,  H'0F42'
        DATA TXB0SIDH,  H'0F41'
        DATA TXB0CON,   H'0F40'


        DATA CANSTAT_RO3, H'0F3E'     ;CANSTAT is repeated

        DATA TXB1D7,    H'0F3D'
        DATA TXB1D6,    H'0F3C'
        DATA TXB1D5,    H'0F3B'
        DATA TXB1D4,    H'0F3A'
        DATA TXB1D3,    H'0F39'
        DATA TXB1D2,    H'0F38'
        DATA TXB1D1,    H'0F37'
        DATA TXB1D0,    H'0F36'

        DATA TXB1DLC,   H'0F35'

        DATA TXB1EIDL,  H'0F34'
        DATA TXB1EIDH,  H'0F33'
        DATA TXB1SIDL,  H'0F32'
        DATA TXB1SIDH,  H'0F31'
        DATA TXB1CON,   H'0F30'


        DATA CANSTAT_RO4, H'0F2E'     ;CANSTAT is repeated

        DATA TXB2D7,    H'0F2D'
        DATA TXB2D6,    H'0F2C'
        DATA TXB2D5,    H'0F2B'
        DATA TXB2D4,    H'0F2A'
        DATA TXB2D3,    H'0F29'
        DATA TXB2D2,    H'0F28'
        DATA TXB2D1,    H'0F27'
        DATA TXB2D0,    H'0F26'

        DATA TXB2DLC,   H'0F25'

        DATA TXB2EIDL,  H'0F24'
        DATA TXB2EIDH,  H'0F23'
        DATA TXB2SIDL,  H'0F22'
        DATA TXB2SIDH,  H'0F21'
        DATA TXB2CON,   H'0F20'

        DATA RXM1EIDL,  H'0F1F'
        DATA RXM1EIDH,  H'0F1E'  
        DATA RXM1SIDL,  H'0F1D'
        DATA RXM1SIDH,  H'0F1C'

        DATA RXM0EIDL,  H'0F1B'
        DATA RXM0EIDH,  H'0F1A'
        DATA RXM0SIDL,  H'0F19'
        DATA RXM0SIDH,  H'0F18'

        DATA RXF5EIDL,  H'0F17'
        DATA RXF5EIDH,  H'0F16'
        DATA RXF5SIDL,  H'0F15'
        DATA RXF5SIDH,  H'0F14'

        DATA RXF4EIDL,  H'0F13'
        DATA RXF4EIDH,  H'0F12'
        DATA RXF4SIDL,  H'0F11'
        DATA RXF4SIDH,  H'0F10'

        DATA RXF3EIDL,  H'0F0F'
        DATA RXF3EIDH,  H'0F0E'
        DATA RXF3SIDL,  H'0F0D'
        DATA RXF3SIDH,  H'0F0C'

        DATA RXF2EIDL,  H'0F0B'
        DATA RXF2EIDH,  H'0F0A'
        DATA RXF2SIDH,  H'0F08'

        DATA RXF1EIDL,  H'0F07'
        DATA RXF1EIDH,  H'0F06'
        DATA RXF1SIDL,  H'0F05'
        DATA RXF1SIDH,  H'0F04'

        DATA RXF0EIDL,  H'0F03'
        DATA RXF0EIDH,  H'0F02'
        DATA RXF0SIDL,  H'0F01'
        DATA RXF0SIDH,  H'0F00'


;----- STKPTR Bits --------------------------------------------------------
        DATA STKFUL,    H'0007'
        DATA STKUNF,    H'0006'

;----- INTCON Bits --------------------------------------------------------
        DATA GIE,       H'0007'
        DATA GIEH,      H'0007'
        DATA PEIE,      H'0006'
        DATA GIEL,      H'0006'
        DATA TMR0IE,    H'0005'
        DATA T0IE,      H'0005'      ; For backward compatibility
        DATA INT0IE,    H'0004'
        DATA INT0E,     H'0004'      ; For backward compatibility
        DATA RBIE,      H'0003'
        DATA TMR0IF,    H'0002'
        DATA T0IF,      H'0002'      ; For backward compatibility
        DATA INT0IF,    H'0001'
        DATA INT0F,     H'0001'      ; For backward compatibility
        DATA RBIF,      H'0000'

;----- INTCON2 Bits --------------------------------------------------------
        DATA NOT_RBPU,  H'0007'
        DATA RBPU,      H'0007'
        DATA INTEDG0,   H'0006'
        DATA INTEDG1,   H'0005'
        DATA INTEDG2,   H'0004'
        DATA TMR0IP,    H'0002'
        DATA T0IP,      H'0002'      ; For compatibility with T0IE and T0IF
        DATA RBIP,      H'0000'

;----- INTCON3 Bits --------------------------------------------------------
        DATA INT2IP,    H'0007'
        DATA INT1IP,    H'0006'
        DATA INT2IE,    H'0004'
        DATA INT1IE,    H'0003'
        DATA INT2IF,    H'0001'
        DATA INT1IF,    H'0000'

;----- STATUS Bits --------------------------------------------------------
        DATA N,         H'0004'
        DATA OV,        H'0003'
        DATA Z,         H'0002'
        DATA DC,        H'0001'
        DATA C,         H'0000'

;----- T0CON Bits ---------------------------------------------------------
        DATA TMR0ON,   H'0007'
        DATA T08BIT,   H'0006'
        DATA T0CS,     H'0005'
        DATA T0SE,     H'0004'
        DATA PSA,      H'0003'
        DATA T0PS2,    H'0002'
        DATA T0PS1,    H'0001'
        DATA T0PS0,    H'0000'

;----- OSCON Bits ---------------------------------------------------------
        DATA SCS,      H'0000'

;----- LVDCON Bits ---------------------------------------------------------
        DATA IRVST,    H'0005'
        DATA LVDEN,    H'0004'
        DATA LVDL3,    H'0003'
        DATA LVDL2,    H'0002'
        DATA LVDL1,    H'0001'
        DATA LVDL0,    H'0000'

;----- WDTCON Bits ---------------------------------------------------------
        DATA SWDTEN,   H'0000'

;----- RCON Bits -----------------------------------------------------------
        DATA IPEN,     H'0007'
        DATA LWRT,     H'0006'
        DATA NOT_RI,   H'0004'
        DATA RI,       H'0004'
        DATA NOT_TO,  H'0003'
        DATA TO,       H'0003'
        DATA NOT_PD,   H'0002'
        DATA PD,       H'0002'
        DATA NOT_POR,  H'0001'
        DATA POR,      H'0001'
        DATA NOT_BOR, H'0000'
        DATA BOR,      H'0000'

;----- T1CON Bits ---------------------------------------------------------
        DATA RD16,      H'0007'
        DATA T1CKPS1,   H'0005'
        DATA T1CKPS0,   H'0004'
        DATA T1OSCEN,   H'0003'
        DATA NOT_T1SYNC, H'0002'
        DATA T1SYNC,    H'0002'
        DATA T1INSYNC,  H'0002'    ; Backward compatibility only
        DATA TMR1CS,    H'0001'
        DATA TMR1ON,    H'0000'

;----- T2CON Bits ---------------------------------------------------------
        DATA TOUTPS3,  H'0006'
        DATA TOUTPS2,  H'0005'
        DATA TOUTPS1,  H'0004'
        DATA TOUTPS0,  H'0003'
        DATA TMR2ON,   H'0002'
        DATA T2CKPS1,  H'0001'
        DATA T2CKPS0,  H'0000'

;----- SSPSTAT Bits -------------------------------------------------------
        DATA SMP,      H'0007'
        DATA CKE,      H'0006'
        DATA D,        H'0005'
        DATA I2C_DAT,  H'0005'
        DATA NOT_A,    H'0005'
        DATA NOT_ADDRESS,   H'0005'
        DATA D_A,           H'0005'
        DATA DATA_ADDRESS,  H'0005'
        DATA P,             H'0004'
        DATA I2C_STOP,      H'0004'
        DATA S,             H'0003'
        DATA I2C_START,     H'0003'
        DATA R,             H'0002'
        DATA I2C_READ,      H'0002'
        DATA NOT_W,         H'0002'
        DATA NOT_WRITE,     H'0002'
        DATA R_W,           H'0002'
        DATA READ_WRITE,    H'0002'
        DATA UA,            H'0001'
        DATA BF,            H'0000'

;----- SSPCON1 Bits --------------------------------------------------------
        DATA WCOL,          H'0007'
        DATA SSPOV,         H'0006'
        DATA SSPEN,         H'0005'
        DATA CKP,           H'0004'
        DATA SSPM3,         H'0003'
        DATA SSPM2,         H'0002'
        DATA SSPM1,         H'0001'
        DATA SSPM0,         H'0000'

;----- SSPCON2 Bits --------------------------------------------------------
        DATA GCEN,          H'0007'
        DATA ACKSTAT,       H'0006'
        DATA ACKDT,         H'0005'
        DATA ACKEN,         H'0004'
        DATA RCEN,          H'0003'
        DATA PEN,           H'0002'
        DATA RSEN,          H'0001'
        DATA SEN,           H'0000'

;----- ADCON0 Bits --------------------------------------------------------
        DATA ADCS1,         H'0007'
        DATA ADCS0,         H'0006'
        DATA CHS2,          H'0005'
        DATA CHS1,          H'0004'
        DATA CHS0,          H'0003'
        DATA GO,            H'0002'
        DATA NOT_DONE,      H'0002'
        DATA DONE,          H'0002'
        DATA GO_DONE,       H'0002'
        DATA ADON,          H'0000'

;----- ADCON1 Bits --------------------------------------------------------
        DATA ADFM,          H'0007'
        DATA ADCS2,         H'0006'
        DATA PCFG3,         H'0003'
        DATA PCFG2,         H'0002'
        DATA PCFG1,         H'0001'
        DATA PCFG0,         H'0000'

;----- CCP1CON Bits -------------------------------------------------------
        DATA DC1B1,         H'0005'
        DATA CCP1X,         H'0005'      ; For backward compatibility
        DATA DC1B0,         H'0004'
        DATA CCP1Y,         H'0004'      ; For backward compatibility
        DATA CCP1M3,        H'0003'
        DATA CCP1M2,        H'0002'
        DATA CCP1M1,        H'0001'
        DATA CCP1M0,        H'0000'

;----- CCP2CON Bits -------------------------------------------------------
        DATA DC2B1,         H'0005'
        DATA CCP2X,         H'0005'      ; For backward compatibility
        DATA DC2B0,         H'0004'
        DATA CCP2Y,         H'0004'      ; For backward compatibility
        DATA CCP2M3,        H'0003'
        DATA CCP2M2,        H'0002'
        DATA CCP2M1,        H'0001'
        DATA CCP2M0,        H'0000'

;----- T3CON Bits ---------------------------------------------------------
        DATA RD16,          H'0007'
        DATA T3CCP2,        H'0006'
        DATA T3CKPS1,       H'0005'
        DATA T3CKPS0,       H'0004'
        DATA T3CCP1,        H'0003'
        DATA NOT_T3SYNC,    H'0002'
        DATA T3SYNC,        H'0002'
        DATA T3INSYNC,      H'0002'    ; Backward compatibility only
        DATA TMR3CS,        H'0001'
        DATA TMR3ON,        H'0000'

;----- TXSTA Bits ---------------------------------------------------------
        DATA CSRC,          H'0007'
        DATA TX9,           H'0006'
        DATA TXEN,          H'0005'
        DATA SYNC,          H'0004'
        DATA BRGH,          H'0002'
        DATA TRMT,          H'0001'
        DATA TX9D,          H'0000'

;----- RCSTA Bits ---------------------------------------------------------
        DATA SPEN,          H'0007'
        DATA RX9,           H'0006'
        DATA SREN,          H'0005'
        DATA CREN,          H'0004'
        DATA ADEN,          H'0003'
        DATA FERR,          H'0002'
        DATA OERR,          H'0001'
        DATA RX9D,          H'0000'

;----- IPR2 Bits ----------------------------------------------------------
        DATA BCLIP,        H'0003'
        DATA LVDIP,        H'0002'
        DATA TMR3IP,       H'0001'
        DATA CCP2IP,       H'0000'
;----- PIR2 Bits ----------------------------------------------------------
        DATA BCLIF,              H'0003'
        DATA LVDIF,              H'0002'
        DATA TMR3IF,             H'0001'
        DATA CCP2IF,             H'0000'

;----- PIE2 Bits ----------------------------------------------------------
        DATA BCLIE,              H'0003'
        DATA LVDIE,              H'0002'
        DATA TMR3IE,             H'0001'
        DATA CCP2IE,             H'0000'

;----- IPR1 Bits ----------------------------------------------------------
        DATA PSPIP,              H'0007'
        DATA ADIP,               H'0006'
        DATA RCIP,               H'0005'
        DATA TXIP,               H'0004'
        DATA SSPIP,              H'0003'
        DATA CCP1IP,             H'0002'
        DATA TMR2IP,             H'0001'
        DATA TMR1IP,             H'0000'

;----- PIR1 Bits ----------------------------------------------------------
        DATA PSPIF,              H'0007'
        DATA ADIF,               H'0006'
        DATA RCIF,               H'0005'
        DATA TXIF,               H'0004'
        DATA SSPIF,              H'0003'
        DATA CCP1IF,             H'0002'
        DATA TMR2IF,             H'0001'
        DATA TMR1IF,             H'0000'

;----- PIE1 Bits ----------------------------------------------------------
        DATA PSPIE,              H'0007'
        DATA ADIE,               H'0006'
        DATA RCIE,               H'0005'
        DATA TXIE,               H'0004'
        DATA SSPIE,              H'0003'
        DATA CCP1IE,             H'0002'
        DATA TMR2IE,             H'0001'
        DATA TMR1IE,             H'0000'

;----- PSPCON Bits ---------------------------------------------------------
        DATA IBF,                H'0007'
        DATA OBF,                H'0006'
        DATA IBOV,               H'0005'
        DATA PSPMODE,            H'0004'

;==========================================================================
;
;       I/O Pin Name Definitions
;
;==========================================================================

;----- PORTA ------------------------------------------------------------------
        DATA RA0,                0
        DATA AN0,                0
        DATA RA1,                1
        DATA AN1,                1
        DATA RA2,                2
        DATA AN2,                2
        DATA VREFP,              2
        DATA RA3,                3
        DATA AN3,                3
        DATA VREFM,              3
        DATA RA4,                4
        DATA T0CKI,              4
        DATA RA5,                5
        DATA AN4,                5
        DATA SS,                 5
        DATA LVDIN,              5
        DATA RA6,                6


;----- PORTB ------------------------------------------------------------------
        DATA RB0,                0
        DATA INT0,               0
        DATA RB1,                1
        DATA INT1,               1
        DATA RB2,                2
        DATA INT2,               2
        DATA RB3,                3
        DATA INT3,               3
        DATA RB4,                4
        DATA RB5,                5
        DATA RB6,                6
        DATA RB7,                7


;----- PORTC ------------------------------------------------------------------
        DATA RC0,                0
        DATA T1OSO,              0
        DATA T1CKI,              0
        DATA RC1,                1
        DATA T1OSI,              1
        DATA CCP2,               1
        DATA RC2,                2
        DATA CCP1,               2
        DATA RC3,                3
        DATA SCK,                3
        DATA SCL,                3
        DATA RC4,                4
        DATA SDI,                4
        DATA SDA,                4
        DATA RC5,                5
        DATA SDO,                5
        DATA RC6,                6
        DATA TX,                 6
        DATA CK,                 6
        DATA RC7,                7
        DATA RX,                 7

;----- PORTD ------------------------------------------------------------------
        DATA RD0,                0
        DATA PSP0,               0
        DATA RD1,                1
        DATA PSP1,               1
        DATA RD2,                2
        DATA PSP2,               2
        DATA RD3,                3
        DATA PSP3,               3
        DATA RD4,                4
        DATA PSP4,               4
        DATA RD5,                5
        DATA PSP5,               5
        DATA RD6,                6
        DATA PSP6,               6
        DATA RD7,                7
        DATA PSP7,               7


;----- PORTE ------------------------------------------------------------------
        DATA RE0,                 0
;;;; Begin: Removed in gputils
;;;;        DATA ALE,                 0
;;;; End: Removed in gputils
        DATA AN5,                 0
        DATA RE1,                 1
;;;; Begin: Removed in gputils
;;;;        DATA OE,                  1
;;;; End: Removed in gputils
        DATA RE2,                 2
;;;; Begin: Removed in gputils
;;;;        DATA WRL,                 2
;;;; End: Removed in gputils
        DATA RE3,                 3
;;;; Begin: Removed in gputils
;;;;        DATA WRH,                 3
;;;; End: Removed in gputils
        DATA RE4,                 4
        DATA RE5,                 5
        DATA RE6,                 6
        DATA RE7,                 7
        DATA CCP2,                7


;----- PORTF ------------------------------------------------------------------

        DATA RF0,                 0
        DATA AN5,                 0
        DATA RF1,                 1
        DATA AN6,                 1
        DATA RF2,                 2
        DATA AN7,                 2
        DATA RF3,                 3
        DATA AN8,                 3
        DATA RF4,                 4
        DATA AN9,                 4
        DATA RF5,                 5
        DATA AN10,                5
        DATA RF6,                 6
        DATA AN11,                6
        DATA RF7,                 7


;----- PORTG ------------------------------------------------------------------
        DATA RG0,                 0
        DATA CANTX1,              0
        DATA RG1,                 1
        DATA CANTX2,              1
        DATA RG2,                 2
        DATA CANRX,               2
        DATA RG3,                 3
        DATA RG4,                 4


;----- PORTH ------------------------------------------------------------------
        DATA RH0,                 0
        DATA RH1,                 1
        DATA RH2,                 2
        DATA RH3,                 3
        DATA RH4,                 4
        DATA RH5,                 5
        DATA RH6,                 6
        DATA RH7,                 7


;----- PORTJ ------------------------------------------------------------------
        DATA RJ0,                 0
;;;; Begin: Removed in gputils
;;;;        DATA AD8,                 0
;;;; End: Removed in gputils
        DATA RJ1,                 1
;;;; Begin: Removed in gputils
;;;;        DATA AD9,                 1
;;;; End: Removed in gputils
        DATA RJ2,                 2
;;;; Begin: Removed in gputils
;;;;        DATA AD10,                2
;;;; End: Removed in gputils
        DATA RJ3,                 3
;;;; Begin: Removed in gputils
;;;;        DATA AD11,                3
;;;; End: Removed in gputils
        DATA RJ4,                 4
;;;; Begin: Removed in gputils
;;;;        DATA AD12,                4
;;;; End: Removed in gputils
        DATA RJ5,                 5
;;;; Begin: Removed in gputils
;;;;        DATA AD13,                5
;;;; End: Removed in gputils
        DATA RJ6,                 6
;;;; Begin: Removed in gputils
;;;;        DATA AD14,                6
;;;; End: Removed in gputils
        DATA RJ7,                 7
;;;; Begin: Removed in gputils
;;;;        DATA AD15,                7
;;;; End: Removed in gputils


;----- PORTK ------------------------------------------------------------------
        DATA RK0,                 0
        DATA RK1,                 1
        DATA RK2,                 2
        DATA RK3,                 3
        DATA RK4,                 4
        DATA RK5,                 5
        DATA RK6,                 6
        DATA RK7,                 7


	END
