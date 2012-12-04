; HD252.ASM

; This file tests header file usage for the specified processor.

        LIST    P=18C252
;;;; Begin: Changed in gputils
;;;;        INCLUDE "p18c252.inc"
        INCLUDE "P18C252.INC"
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
        DATA FSR2L,   H'0FD9'
        DATA STATUS,  H'0FD8'

        DATA TMR0H,   H'0FD7'
        DATA TMR0L,   H'0FD6'
        DATA T0CON,   H'0FD5'

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


        DATA TRISC,     H'0F94'
        DATA TRISB,     H'0F93'
        DATA TRISA,     H'0F92'


        DATA LATC,      H'0F8B'
        DATA LATB,      H'0F8A'
        DATA LATA,      H'0F89'

        DATA PORTC,     H'0F82'
        DATA PORTB,     H'0F81'
        DATA PORTA,     H'0F80'

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
        DATA TX8_9,         H'0006'    ; Backward compatibility only
        DATA TXEN,          H'0005'
        DATA SYNC,          H'0004'
        DATA BRGH,          H'0002'
        DATA TRMT,          H'0001'
        DATA TX9D,          H'0000'
        DATA TXD8,          H'0000'    ; Backward compatibility only

;----- RCSTA Bits ---------------------------------------------------------
        DATA SPEN,          H'0007'
        DATA RX9,           H'0006'
        DATA RC9,           H'0006'    ; Backward compatibility only
        DATA NOT_RC8,       H'0006'    ; Backward compatibility only
        DATA RC8_9,         H'0006'    ; Backward compatibility only
        DATA SREN,          H'0005'
        DATA CREN,          H'0004'
        DATA ADDEN,         H'0003'
        DATA FERR,          H'0002'
        DATA OERR,          H'0001'
        DATA RX9D,          H'0000'
        DATA RCD8,          H'0000'    ; Backward compatibility only

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
;;;; Begin: Removed in gputils
;;;;        DATA PSPIP,              H'0007'
;;;; End: Removed in gputils
        DATA ADIP,               H'0006'
        DATA RCIP,               H'0005'
        DATA TXIP,               H'0004'
        DATA SSPIP,              H'0003'
        DATA CCP1IP,             H'0002'
        DATA TMR2IP,             H'0001'
        DATA TMR1IP,             H'0000'

;----- PIR1 Bits ----------------------------------------------------------
;;;; Begin: Removed in gputils
;;;;        DATA PSPIF,              H'0007'
;;;; End: Removed in gputils
        DATA ADIF,               H'0006'
        DATA RCIF,               H'0005'
        DATA TXIF,               H'0004'
        DATA SSPIF,              H'0003'
        DATA CCP1IF,             H'0002'
        DATA TMR2IF,             H'0001'
        DATA TMR1IF,             H'0000'

;----- PIE1 Bits ----------------------------------------------------------
;;;; Begin: Removed in gputils
;;;;        DATA PSPIE,              H'0007'
;;;; End: Removed in gputils
        DATA ADIE,               H'0006'
        DATA RCIE,               H'0005'
        DATA TXIE,               H'0004'
        DATA SSPIE,              H'0003'
        DATA CCP1IE,             H'0002'
        DATA TMR2IE,             H'0001'
        DATA TMR1IE,             H'0000'

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
        DATA VREFP,              3
        DATA RA3,                3
        DATA AN3,                3
        DATA VREFM,              2
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
        DATA CCP2A,              3
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
;;;; Begin: Changed in gputils
;;;;        DATA CCP2,               1
        DATA CCP2_PORTC,         1
;;;; End: Changed in gputils
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


	END
