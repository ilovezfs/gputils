; HD756.ASM

; This file tests header file usage for the specified processor.

	LIST	P=17C756
	INCLUDE	"p17c756.inc"

 	DATA	_PMC_MODE,	0X7FAF		; This changes per device
	DATA	_XMC_MODE,	0XFFBF
	DATA	_MC_MODE,	0XFFEF
	DATA	_MP_MODE,	0XFFFF

    DATA    _BODEN_OFF, 0xBFFF
    DATA    _BODEN_ON,  0xFFFF

	DATA	_WDT_NORM,	0XFFF3
    DATA    _WDT_OFF,   0XFFF3
	DATA	_WDT_64,	0XFFF7
	DATA	_WDT_256,	0XFFFB
	DATA	_WDT_0,		0XFFFF
	DATA	_LF_OSC,	0XFFFC
	DATA	_RC_OSC,	0XFFFD
	DATA	_XT_OSC,	0XFFFE
	DATA	_EC_OSC,	0XFFFF

    DATA    BANK0,      0
    DATA    BANK1,      1
    DATA    BANK2,      2
    DATA    BANK3,      3
    DATA    BANK4,      4
    DATA    BANK5,      5
    DATA    BANK6,      6
    DATA    BANK7,      7

    DATA    GPR_BANK0,  0X00
    DATA    GPR_BANK1,  0X08
    DATA    GPR_BANK2,  0X10
    DATA    GPR_BANK3,  0X18

	DATA	INDF0,      0			; Register files
	DATA	FSR0,       1			; Unbanked
	DATA	PCL,        2
	DATA	PCLATH,     3
	DATA	ALUSTA,     4
	DATA	T0STA,      5
	DATA	CPUSTA,     6
	DATA	INTSTA,     7
	DATA	INDF1,      8
	DATA	FSR1,       9
	DATA	WREG,       0XA
	DATA	TMR0L,      0XB
	DATA	TMR0H,      0XC
	DATA	TBLPTRL,    0XD
	DATA	TBLPTRH,    0XE
	DATA	BSR,        0XF

	DATA	PORTA,      0X0010		; Bank 0
	DATA	DDRB,       0X0011
	DATA	PORTB,      0X0012	
	DATA	RCSTA,      0X0013
	DATA	RCREG,      0X0014
    DATA    RCREG1,     0X0014
	DATA	TXSTA,      0X0015
    DATA    TXSTA1,     0X0015
	DATA	TXREG,      0X0016
    DATA    TXREG1,     0X0016
	DATA	SPBRG,      0X0017
    DATA    SPBRG1,     0X0017

	DATA	DDRC,       0X0110		; Bank 1
	DATA	PORTC,      0X0111
	DATA 	DDRD,       0X0112
	DATA	PORTD,      0X0113
	DATA	DDRE,       0X0114
	DATA	PORTE,      0X0115
	DATA	PIR,        0X0116
    DATA    PIR1,       0x0116
	DATA	PIE,        0X0117
    DATA    PIE1,       0x0117

	DATA	TMR1,       0X0210		; Bank 2
	DATA	TMR2,       0X0211
	DATA	TMR3L,      0X0212
	DATA	TMR3H,      0X0213
	DATA	PR1,        0X0214
	DATA	PR2,        0X0215
	DATA	PR3L,       0X0216
	DATA	PR3H,       0X0217
    DATA    CA1L,       0X0216
    DATA    CA1H,       0X0217

	DATA	PW1DCL,     0X0310		; Bank 3
	DATA	PW2DCL,     0X0311
	DATA	PW1DCH,     0X0312
	DATA	PW2DCH,     0X0313
	DATA 	CA2L,       0X0314
	DATA	CA2H,       0X0315
	DATA	TCON1,      0X0316
	DATA	TCON2,      0X0317

    DATA    PIR2,       0X0410      ; Bank 4
    DATA    PIE2,       0X0411
    DATA    RCSTA2,     0X0413
    DATA    RCREG2,     0X0414
    DATA    TXSTA2,     0X0415
    DATA    TXREG2,     0X0416
    DATA    SPBRG2,     0X0417
    
    DATA    DDRF,       0X0510      ; Bank 5
    DATA    PORTF,      0X0511
    DATA    DDRG,       0X0512
    DATA    PORTG,      0X0513
    DATA    ADCON0,     0X0514
    DATA    ADCON1,     0X0515
    DATA    ADRESL,     0X0516
    DATA    ADRESH,     0X0517

    DATA    SSPADD,     0X0610      ; Bank 6
    DATA    SSPCON1,    0X0611
    DATA    SSPCON2,    0X0612
    DATA    SSPSTAT,    0X0613
    DATA    SSPBUF,     0X0614

    DATA    PW3DCL,     0X0710      ; Bank 7
    DATA    PW3DCH,     0X0711
    DATA    CA3L,       0X0712
    DATA    CA3H,       0X0713
    DATA    CA4L,       0X0714
    DATA    CA4H,       0X0715
    DATA    TCON3,      0X0716

    DATA    PRODL,      0X0018      ; Unbanked
    DATA    PL,         0X0018
    DATA    PRODH,      0X0019
    DATA    PH,         0X0019

	DATA	FS3,        7			; ALU status Register
	DATA	FS2,        6
	DATA	FS1,        5
	DATA	FS0,        4
	DATA	OV,         3
	DATA	Z,          2
	DATA	DC,         1
	DATA	C,          0

	DATA	STKAV,      5		    ; CPU Status Register
	DATA 	GLINTD,     4
	DATA	NOT_TO,     3
        DATA    TO,         3
	DATA	NOT_PD,     2
        DATA    PD,         2
        DATA    NOT_POR,    1
        DATA    POR,        1
        DATA    NOT_BOR,    0
        DATA    BOR,        0

	DATA	PEIF,       7			; Interrupt Status Register
	DATA	T0CKIF,     6
	DATA	T0IF,       5
	DATA	INTF,       4
	DATA	PEIE,       3
	DATA	T0CKIE,     2
	DATA	T0IE,       1
	DATA	INTE,       0	

	DATA	RBIF,       7			; PIR1 Register
	DATA	TMR3IF,     6
	DATA	TMR2IF,     5
	DATA	TMR1IF,     4
	DATA	CA2IF,      3
	DATA	CA1IF,      2
	DATA	TXIF,       1
	DATA	RCIF,       0

    DATA    RBIE,       7           ; PIE1 Register
    DATA    TMR3IE,     6
    DATA    TMR2IE,     5
    DATA    TMR1IE,     4
    DATA    CA2IE,      3
    DATA    CA1IE,      2
    DATA    TXIE,       1
    DATA    TX1IE,      1
    DATA    RCIE,       0
    DATA    RC1IE,      0

	DATA	NOT_RBPU,   7           ; Port A
    DATA    RBPU,       7

	DATA	SPEN,       7			; RCSTA1 AND 2 Bits
	DATA	RC9,        6
    DATA    RX9,        6
	DATA	NOT_RC8,    6
	DATA	RC8_9,      6
	DATA	SREN,       5
	DATA	CREN,       4
	DATA	FERR,       2
	DATA	OERR,       1
    DATA    RX9D,       0
	DATA	RCD8,       0

	DATA	INTEDG,     7           ; T0STA
	DATA	T0SE,       6
	DATA	T0CS,       5
	DATA	T0PS3,      4
    DATA    PS3,        4
	DATA	T0PS2,      3
    DATA    PS2,        3
	DATA	T0PS1,      2
    DATA    PS1,        2
	DATA	T0PS0,      1
    DATA    PS0,        1

	DATA	CA2ED1,     7		    ; TCON1
	DATA	CA2ED0,     6
	DATA	CA1ED1,     5
	DATA	CA1ED0,     4
	DATA	T16,        3
	DATA	TMR3CS,     2
	DATA	TMR2CS,     1
	DATA	TMR1CS,     0

	DATA	CA2OVF,     7		    ; TCON2
	DATA	CA1OVF,     6
	DATA	PWM2ON,     5
	DATA	PWM1ON,     4
	DATA	CA1,        3
	DATA	NOT_PR3,    3
    DATA    PR3,        3
	DATA	CA1_PR3,    3
	DATA	TMR3ON,     2
	DATA	TMR2ON,     1
	DATA	TMR1ON,     0

    DATA    SSPIF,      7           ; PIR2 Bits
    DATA    BCLIF,      6
    DATA    ADIF,       5
    DATA    CA4IF,      3
    DATA    CA3IF,      2
    DATA    TX2IF,      1
    DATA    RC2IF,      0

    DATA    SSPIE,      7           ; PIE2 Bits
    DATA    BCLIE,      6
    DATA    ADIE,       5
    DATA    CA4IE,      3
    DATA    CA3IE,      2
    DATA    TX2IE,      1
    DATA    RC2IE,      0

    DATA    CSRC,       7           ; TXSTA1 and 2 Bits
    DATA    TX9,        6
    DATA    NOT_TX8,    6
    DATA    TX8_9,      6
    DATA    TXEN,       5
    DATA    SYNC,       4
    DATA    TRMT,       1
    DATA    TX9D,       0
    DATA    TXD8,       0

    DATA    CHS3,       7           ; ADCON0 Bits
    DATA    CHS2,       6
    DATA    CHS1,       5
    DATA    CHS0,       4
    DATA    GO,         2
    DATA    NOT_DONE,   2
    DATA    DONE,       2
    DATA    ADON,       0

    DATA    ADCS1,      7           ; ADCON 1 Bits
    DATA    ADCS0,      6
    DATA    ADFM,       5
    DATA    PCFG3,      3
    DATA    PCFG2,      2
    DATA    PCFG1,      1
    DATA    PCFG0,      0

    DATA    WCOL,       7           ; SSPCON1 Bits
    DATA    SSPOV,      6
    DATA    SSPEN,      5
    DATA    CKP,        4
    DATA    SSPM3,      3
    DATA    SSPM2,      2
    DATA    SSPM1,      1
    DATA    SSPM0,      0

    DATA    GCEN,       7           ; SSPCON2 Bits
    DATA    AKSTAT,     6
    DATA    AKDT,       5
    DATA    AKEN,       4
    DATA    RCEN,       3
    DATA    PEN,        2
    DATA    RSEN,       1
    DATA    SEN,        0

    DATA    SMP,        7           ; SSPSTAT Bits
    DATA    CKE,        6
    DATA    D,          5
    DATA    NOT_A,      5
    DATA    D_A,        5
    DATA    P,          4
    DATA    S,          3
    DATA    R,          2
    DATA    NOT_W,      2
    DATA    R_W,        2
    DATA    UA,         1
    DATA    BF,         0

    DATA    CA4OVF,     6           ; TCON3 Bits
    DATA    CA3OVF,     5
    DATA    CA4ED1,     4
    DATA    CA4ED0,     3
    DATA    CA3ED1,     2
    DATA    CA3ED0,     1
    DATA    PWM3ON,     0

	END
