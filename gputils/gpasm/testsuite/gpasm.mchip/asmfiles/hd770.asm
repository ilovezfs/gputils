; HD770.ASM

; This file tests header file usage for the specified processor.

        LIST    P=16C770
        INCLUDE "p16c770.inc"

        DATA _BODEN_ON,      H'3FFF'  ;this changes per device
        DATA _BODEN_OFF,     H'3FBF'
        DATA _CP_ALL,        H'0CFF'
        DATA _CP_OFF,        H'3FFF'
        DATA _VBOR_25,       H'3FFF'
        DATA _VBOR_27,       H'3BFF'
        DATA _VBOR_42,       H'37FF'
        DATA _VBOR_45,       H'33FF'
        DATA _PWRTE_OFF,     H'3FFF'
        DATA _PWRTE_ON,      H'3FEF'
        DATA _WDT_ON,        H'3FFF'
        DATA _WDT_OFF,       H'3FF7'
        DATA _ER_OSC_CLKOUT,     H'3FFF'
        DATA _ER_OSC_NOCLKOUT,   H'3FFE'
        DATA _INTRC_OSC_CLKOUT,  H'3FFD'
        DATA _INTRC_OSC_NOCLKOUT, H'3FFC'
        DATA _EXTCLK_OSC,     H'3FFB'
        DATA _HS_OSC,         H'3FFA'
        DATA _XT_OSC,         H'3FF9'
        DATA _LP_OSC,         H'3FF8'

;----- Register Files------------------------------------------------------
                                             
        DATA INDF,      H'0000'
        DATA TMR0,      H'0001'
        DATA PCL,       H'0002'
        DATA STATUS,    H'0003'
        DATA FSR,       H'0004'
        DATA PORTA,      H'0005'
        DATA PORTB,      H'0006'
        DATA PCLATH,     H'000A'
        DATA INTCON,     H'000B'
        DATA PIR1,       H'000C'
        DATA PIR2,       H'000D'
        DATA TMR1L,      H'000E'
        DATA TMR1H,      H'000F'
        DATA T1CON,       H'0010'
        DATA TMR2,        H'0011'
        DATA T2CON,       H'0012'
        DATA SSPBUF,      H'0013'
        DATA SSPCON,      H'0014'
        DATA CCPR1L,      H'0015'
        DATA CCPR1H,      H'0016'
        DATA CCP1CON,     H'0017'
        DATA ADRESH,      H'001E'
        DATA ADCON0,      H'001F'

        DATA OPTION_REG,     H'0081'
        DATA TRISA,          H'0085'
        DATA TRISB,          H'0086'
        DATA PIE1,           H'008C'
        DATA PIE2,           H'008D'
        DATA PCON,           H'008E'
        DATA SSPCON2,        H'0091'
        DATA PR2,            H'0092'
        DATA SSPADD,         H'0093'
        DATA SSPSTAT,        H'0094'
        DATA WPUB,           H'0095'
        DATA IOCB,           H'0096'
        DATA P1DEL,          H'0097'
        DATA REFCON,         H'009B'
        DATA LVDCON,         H'009C'
        DATA ANSEL,          H'009D'
        DATA ADRESL,         H'009E'
        DATA ADCON1,          H'009F'
        DATA INDF,            H'0000'
        DATA TMR0,           H'0001'
        DATA PCL,            H'0002'
        DATA STATUS,         H'0003'
        DATA FSR,            H'0004'
        DATA PORTA,          H'0005'
        DATA PORTB,          H'0006'
        DATA PCLATH,         H'000A'
        DATA INTCON,         H'000B'
        DATA PIR1,           H'000C'
        DATA PIR2,           H'000D'
        DATA TMR1L,          H'000E'
        DATA TMR1H,           H'000F'
        DATA T1CON,           H'0010'
        DATA TMR2,         H'0011'
        DATA T2CON,         H'0012'
        DATA SSPBUF,        H'0013'
        DATA SSPCON,        H'0014'
        DATA CCPR1L,        H'0015'
        DATA CCPR1H,        H'0016'
        DATA CCP1CON,       H'0017'
        DATA ADRESH,        H'001E'
        DATA ADCON0,        H'001F'

        DATA OPTION_REG,      H'0081'
        DATA TRISA,           H'0085'
        DATA TRISB,           H'0086'
        DATA PIE1,            H'008C'
        DATA PIE2,            H'008D'
        DATA PCON,            H'008E'
        DATA SSPCON2,         H'0091'
        DATA PR2,             H'0092'
        DATA SSPADD,          H'0093'
        DATA SSPSTAT,         H'0094'
        DATA WPUB,            H'0095'
        DATA IOCB,            H'0096'
        DATA P1DEL,            H'0097'
        DATA REFCON,           H'009B'
        DATA LVDCON,           H'009C'
        DATA ANSEL,            H'009D'
        DATA ADRESL,           H'009E'
        DATA ADCON1,           H'009F'

;----- STATUS Bits --------------------------------------------------------

        DATA IRP,              H'0007'
        DATA RP1,              H'0006'
        DATA RP0,              H'0005'
        DATA NOT_TO,           H'0004'
        DATA NOT_PD,           H'0003'
        DATA Z,                H'0002'
        DATA DC,               H'0001'
        DATA C,                H'0000'

;----- INTCON Bits --------------------------------------------------------

        DATA GIE,          H'0007'
        DATA PEIE,         H'0006'
        DATA T0IE,         H'0005'
        DATA INTE,         H'0004'
        DATA RBIE,         H'0003'
        DATA T0IF,         H'0002'
        DATA INTF,         H'0001'
        DATA RBIF,         H'0000'

;----- PIR1 Bits ----------------------------------------------------------

        DATA ADIF,          H'0006'
        DATA SSPIF,         H'0003'
        DATA CCP1IF,        H'0002'
        DATA TMR2IF,        H'0001'
        DATA TMR1IF,        H'0000'

;----- PIR2 Bits ----------------------------------------------------------

        DATA LVDIF,         H'0007'
        DATA BCLIF,         H'0003'

;----- T1CON Bits ---------------------------------------------------------

        DATA T1CKPS1,       H'0005'
        DATA T1CKPS0,       H'0004'
        DATA T1OSCEN,         H'0003'
        DATA NOT_T1SYNC,      H'0002'
        DATA T1INSYNC,        H'0002'    ; Backward compatibility only
        DATA TMR1CS,          H'0001'
        DATA TMR1ON,          H'0000'

;----- T2CON Bits ---------------------------------------------------------

        DATA TOUTPS3,         H'0006'
        DATA TOUTPS2,         H'0005'
        DATA TOUTPS1,         H'0004'
        DATA TOUTPS0,          H'0003'
        DATA TMR2ON,           H'0002'
        DATA T2CKPS1,          H'0001'
        DATA T2CKPS0,          H'0000'

;----- SSPCON Bits --------------------------------------------------------

        DATA WCOL,             H'0007'
        DATA SSPOV,            H'0006'
        DATA SSPEN,            H'0005'
        DATA SSPM3,             H'0003'
        DATA SSPM2,             H'0002'
        DATA SSPM1,             H'0001'
        DATA SSPM0,             H'0000'

;----- CCP1CON Bits -------------------------------------------------------

        DATA PWM1M1,         H'0007'
        DATA PWM1M0,         H'0006'
        DATA DC1B1,          H'0005'
        DATA DC1B0,          H'0004'
        DATA CCP1M3,         H'0003'
        DATA CCP1M2,         H'0002'
        DATA CCP1M1,         H'0001'
        DATA CCP1M0,         H'0000'

;----- ADCON0 Bits --------------------------------------------------------

        DATA ADCS1,          H'0007'
        DATA ADCS0,          H'0006'
        DATA CHS2,           H'0005'
        DATA CHS1,           H'0004'
        DATA CHS0,           H'0003'
        DATA GO,             H'0002'
        DATA NOT_DONE,       H'0002'
        DATA GO_DONE,        H'0002'
        DATA CHS3,           H'0001'
        DATA ADON,           H'0000'

;----- OPTION_REG Bits ----------------------------------------------------

        DATA NOT_RBPU,       H'0007'
        DATA INTEDG,         H'0006'
        DATA T0CS,           H'0005'
        DATA T0SE,           H'0004'
        DATA PSA,            H'0003'
        DATA PS2,            H'0002'
        DATA PS1,            H'0001'
        DATA PS0,            H'0000'


;----- PIE1 Bits ----------------------------------------------------------

        DATA ADIE,           H'0006'
        DATA SSPIE,          H'0003'
        DATA CCP1IE,         H'0002'
        DATA TMR2IE,         H'0001'
        DATA TMR1IE,         H'0000'

;----- PIE2 Bits ----------------------------------------------------------

        DATA LVDIE,          H'0007'
        DATA BCLIE,          H'0003'

;----- PCON Bits ----------------------------------------------------------

        DATA OSCF,           H'0003'
        DATA NOT_POR,        H'0001'
        DATA NOT_BO,         H'0000'
        DATA NOT_BOR,        H'0000'

;----- SSPCON2 Bits --------------------------------------------------------

        DATA GCEN,           H'0007'
        DATA ACKSTAT,        H'0006'
        DATA ACKDT,          H'0005'
        DATA ACKEN,          H'0004'
        DATA RCEN,           H'0003'
        DATA PEN,            H'0002'
        DATA RSEN,           H'0001'   
        DATA SEN,            H'0000'   

;----- SSPSTAT Bits -------------------------------------------------------

        DATA SMP,            H'0007'
        DATA CKE,            H'0006'
        DATA D,              H'0005'
        DATA I2C_DATA,       H'0005'
        DATA NOT_A,          H'0005'
        DATA NOT_ADDRESS,       H'0005'
        DATA D_A,               H'0005'
        DATA DATA_ADDRESS,      H'0005'
        DATA P,                 H'0004'
        DATA I2C_STOP,          H'0004'
        DATA S,                 H'0003'
        DATA I2C_START,         H'0003'
        DATA R,                 H'0002'
        DATA I2C_READ,        H'0002'
        DATA NOT_W,           H'0002'
        DATA NOT_WRITE,      H'0002'
        DATA R_W,             H'0002'
        DATA READ_WRITE,      H'0002'
        DATA UA,              H'0001'
        DATA BF,              H'0000'

;----- REFCON Bits --------------------------------------------------------

        DATA VRHEN,         H'0007'
        DATA VRLEN,         H'0006'
        DATA VRHOEN,        H'0005'
        DATA VRLOEN,        H'0004'

;----- LVDCON Bits --------------------------------------------------------

        DATA BGST,          H'0005'
        DATA LVDEN,         H'0004'
        DATA LV3,           H'0003'
        DATA LV2,           H'0002'
        DATA LV1,           H'0001'
        DATA LV0,           H'0000'

;----- ADCON1 Bits --------------------------------------------------------

        DATA ADFM,          H'0007'
        DATA VCFG2,         H'0006'
        DATA VCFG1,         H'0005'
        DATA VCFG0,         H'0004'

	END
