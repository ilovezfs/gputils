; HD870.ASM

; This file tests header file usage for the specified processor.

        LIST    P=16F870
;;;; Begin: Changed in gputils
;;;;        INCLUDE "p16f870.inc"
        INCLUDE "P16F870.INC"
;;;; End: Changed in gputils

;;;; Begin: Changed in gputils
;;;;        DATA _CP_ALL,           0x0FCF
        DATA _CP_All,           0x0FCF
;;;; End: Changed in gputils
        DATA _CP_OFF,           0x3FFF
        DATA _DEBUG_ON,         0x37FF
        DATA _DEBUG_OFF,        0x3FFF
        DATA _WRT_ENABLE_ON,    0x3FFF
        DATA _WRT_ENABLE_OFF,   0x3DFF
        DATA _CPD_ON,           0x3EFF
        DATA _CPD_OFF,          0x3FFF
        DATA _LVP_ON,           0x3FFF 
        DATA _LVP_OFF,          0x3F7F 
        DATA _BODEN_ON,         0x3FFF
        DATA _BODEN_OFF,        0x3FBF
        DATA _PWRTE_OFF,        0x3FFF
        DATA _PWRTE_ON,         0x3FF7
        DATA _WDT_ON,           0x3FFF
        DATA _WDT_OFF,          0x3FFB
        DATA _LP_OSC,           0x3FFC
        DATA _XT_OSC,           0x3FFD
        DATA _HS_OSC,           0x3FFE
        DATA _RC_OSC,           0x3FFF


	DATA	INDF,		0	; Register Files
	DATA	TMR0,		1
	DATA	PCL,		2
	DATA	STATUS,		3
	DATA	FSR,		4
	DATA	PORTA,		5
	DATA	PORTB,		6
	DATA	PORTC,		7	; Processor-specific
;       DATA    PORTD,          8
;       DATA    PORTE,          9

	DATA	PCLATH,		0x0A
	DATA	INTCON,		0x0B
	DATA	PIR1,		0X0C
	DATA	PIR2,		0X0D
	DATA	TMR1L,		0X0E
	DATA	TMR1H,		0X0F
	DATA	T1CON,		0X10
	DATA	TMR2,		0X11
	DATA	T2CON,		0X12
	DATA	CCPR1L,		0X15
	DATA	CCPR1H,		0X16
	DATA	CCP1CON,	0X17
        DATA    ADRESL,         0x1E
        DATA    ADRESH,         0x9E
	DATA	ADCON0,		0x1F

	DATA	OPTION_REG,	0x81

	DATA	TRISA,		0X85
	DATA	TRISB,		0X86
	DATA	TRISC,		0X87
;       DATA    TRISD,          0x88
;       DATA    TRISE,          0x89
 
	DATA	PIE1,		0X8C
	DATA	PIE2,		0X8D
        DATA    PCON,           0X8E

        DATA   PR2,             0x92
        DATA   TXSTA,           0x98
        DATA   SPBRG,           0x99

        DATA    ADCON1,         0x9F


        DATA    EEDATA,         0x10C
        DATA    EEADR,          0x10D
        DATA    EEDATH,         0x10E
        DATA    EEADRH,         0x10F

        DATA    EECON1,         0x18C
        DATA    EECON2,         0x18D


;----- STATUS Bits --------------------------------------------------------

	DATA	IRP,		7	; STATUS
	DATA	RP1,		6
	DATA	RP0,		5
	DATA	NOT_TO,		4
	DATA	NOT_PD,		3
	DATA	Z,		2
	DATA	DC,		1
	DATA	C,		0

;----- INTCON Bits --------------------------------------------------------

	DATA	GIE,		7	; INTCON
	DATA	PEIE,		6
	DATA	T0IE,		5
	DATA	INTE,		4
	DATA	RBIE,		3
	DATA	T0IF,		2
	DATA	INTF,		1
	DATA	RBIF,		0

;----- PIR1 Bits ----------------------------------------------------------

        DATA    ADIF,           6       ; PIR1 Bits
        DATA    RCIF,           5
	DATA	TXIF,		4
	DATA	CCP1IF,		2
	DATA	TMR2IF,		1
	DATA	TMR1IF,		0

;----- PIE2 Bits ----------------------------------------------------------

        DATA    EEIE,           4   

;----- PIR2 Bits ----------------------------------------------------------

        DATA    EEIF,           4   

;----- T1CON Bits ---------------------------------------------------------


	DATA	T1CKPS1,	5	; T1CON
	DATA	T1CKPS0,	4
	DATA	T1OSCEN,	3
	DATA	NOT_T1SYNC,	2
	DATA	T1INSYNC,	2
	DATA	TMR1CS,		1
	DATA	TMR1ON,		0

;----- T2CON Bits ---------------------------------------------------------

	DATA	TOUTPS3,	6	; T2CON
	DATA	TOUTPS2,	5
	DATA	TOUTPS1,	4
	DATA	TOUTPS0,	3
	DATA	TMR2ON,		2
	DATA	T2CKPS1,	1
	DATA	T2CKPS0,	0

;----- CCP1CON Bits -------------------------------------------------------

	DATA	CCP1X,		5	; CCP1CON
	DATA	CCP1Y,		4
	DATA	CCP1M3,		3
	DATA	CCP1M2,		2
	DATA	CCP1M1,		1
	DATA	CCP1M0,		0


;----- RCSTA Bits ---------------------------------------------------------

	DATA	SPEN,		7	; RCSTA
	DATA	RX9,		6
	DATA	RC9,		6
	DATA	NOT_RC8,	6
	DATA	RC8_9,		6
	DATA	SREN,		5
	DATA	CREN,		4
	DATA	FERR,		2
	DATA	OERR,		1
	DATA	RX9D,		0
	DATA	RCD8,		0

;----- ADCON0 Bits --------------------------------------------------------

	DATA	ADCS1,		7	; ADCON0
	DATA	ADCS0,		6
	DATA	CHS2,		5
	DATA	CHS1,		4
	DATA	CHS0,		3
	DATA	GO,		2
	DATA	NOT_DONE,	2
	DATA	GO_DONE,	2
	DATA	ADON,		0

;----- OPTION_REG Bits ----------------------------------------------------

	DATA	NOT_RBPU,	7	; OPTION
	DATA	INTEDG,		6
	DATA	T0CS,		5
	DATA	T0SE,		4
	DATA	PSA,		3
	DATA	PS2,		2
	DATA	PS1,		1
	DATA	PS0,		0


;----- PIE1 Bits ----------------------------------------------------------

	DATA	ADIE,		6
	DATA	RCIE,		5
	DATA	TXIE,		4
	DATA	CCP1IE,		2
	DATA	TMR2IE,		1
	DATA	TMR1IE,		0

;----- PCON Bits ----------------------------------------------------------

	DATA	NOT_POR,	1	; PCON
	DATA	NOT_BO,		0
	DATA	NOT_BOR,	0

;----- EECON1 Bits --------------------------------------------------------

        DATA    EEPGD,          7
        DATA    WRERR,          3
        DATA    WREN,           2
        DATA    WR,             1
        DATA    RD,             0


;----- TXSTA Bits ---------------------------------------------------------

	DATA	CSRC,		7	; TXSTA
	DATA	TX9,		6
	DATA	NOT_TX8,	6
	DATA	TX8_9,		6
	DATA	TXEN,		5
	DATA 	SYNC,		4
	DATA	BRGH,		2
	DATA	TRMT,		1
	DATA	TX9D,		0
	DATA	TXD8,		0

;----- ADCON1 Bits --------------------------------------------------------

        DATA    ADFM,           5       ;ADCON1
        DATA    PCFG3,          3
        DATA    PCFG2,          2
        DATA    PCFG1,          1
        DATA    PCFG0,          0

	END
