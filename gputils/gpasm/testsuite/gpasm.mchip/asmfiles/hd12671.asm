; HD671.ASM

; This file tests header file usage for the specified processor.

	LIST	P=12C671
	INCLUDE	"p12c671.inc"

	DATA	_MCLRE_ON                    , H'3FFF'
	DATA	_MCLRE_OFF                   , H'3F7F'
	DATA	_CP_ALL                      , H'009F'
	DATA	_CP_75                       , H'15BF'
	DATA	_CP_50                       , H'2ADF'
	DATA	_CP_OFF                      , H'3FFF'
	DATA	_PWRTE_OFF                   , H'3FFF'
	DATA	_PWRTE_ON                    , H'3FEF'
	DATA	_WDT_ON                      , H'3FFF'
	DATA	_WDT_OFF                     , H'3FF7'
	DATA	_LP_OSC                      , H'3FF8'
	DATA	_XT_OSC                      , H'3FF9'
	DATA	_HS_OSC                      , H'3FFA'
	DATA	_INTRC_OSC                   , H'3FFC'
	DATA	_INTRC_OSC_NOCLKOUT          , H'3FFC'
	DATA	_INTRC_OSC_CLKOUT            , H'3FFD'
	DATA	_EXTRC_OSC                   , H'3FFE'
	DATA	_EXTRC_OSC_NOCLKOUT          , H'3FFE'
	DATA	_EXTRC_OSC_CLKOUT            , H'3FFF'

	DATA	INDF, 		0			;Register Files
	DATA	TMR0,		1
	DATA	PCL, 		2
	DATA	STATUS, 	3
	DATA	FSR,		4
	DATA	GPIO,		5

	DATA	PCLATH, 	0x0A
	DATA	INTCON, 	0x0B
	DATA	PIR1,		0x0C

	DATA	ADRES,		0x1E
	DATA	ADCON0,		0x1F

	DATA	OPTION_REG,	0x81
	DATA	TRISIO,		0X85
	DATA	PIE1,		0x8C
	DATA	PCON,		0x8E
	DATA	OSCCAL,		0x8F
	DATA	ADCON1,		0x9F

	DATA	IRP,		7	; STATUS
	DATA	RP1, 		6
	DATA	RP0, 		5
	DATA	NOT_TO, 	4
	DATA	NOT_PD, 	3
	DATA	Z, 		2
	DATA	DC, 		1
	DATA	C, 		0

	DATA	ADCS1,		7	; ADCON0
	DATA	ADCS0,		6

	DATA	CHS1,		4
	DATA	CHS0,		3
	DATA	GO,		2
	DATA	NOT_DONE,	2
	DATA	GO_DONE,	2
	DATA	ADON,		0

	DATA	GIE,		7	; INTCON
	DATA	PEIE,		6
	DATA	T0IE,		5

	DATA	GPIE,		3
	DATA	T0IF,		2

	DATA	GPIF,		0	

	DATA	ADIF,		6	; PIR1

	DATA	NOT_GPPU,	7	; OPTION
	DATA	INTEDG, 	6
	DATA	T0CS,		5
	DATA	T0SE,		4
	DATA	PSA,		3
	DATA	PS2,		2
	DATA	PS1,		1
	DATA	PS0,		0

	DATA	ADIE,		6	; PIE1

	DATA	NOT_POR,	1	; PCON

	DATA	PCFG2,		2	; ADCON1
	DATA	PCFG1,		1
	DATA	PCFG0,		0

	END
