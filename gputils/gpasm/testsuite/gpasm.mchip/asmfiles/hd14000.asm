; HD14000.ASM

; This file tests header file usage for the specified processor.

	LIST	P=14000
	INCLUDE	"p14000.inc"

 	DATA	_CPC_ON, 0x1E7F		; This changes per device
	DATA	_CPU_ON, 0x2DDF
	DATA	_CPP_ON, 0x33EF
	DATA	_PWRTE_ON, 0x3FF7	; This changes per device

	DATA	TMR0, 1
	DATA	STATUS, 3
	DATA	PCLATH, 0x0A
	DATA	INTCON, 0x0B
	DATA	ADCAPL, 0x15
	DATA	OPTION_REG, 0x81

	DATA	GIE, 7
	DATA	T0IE, 5

	DATA	Z, 2
	DATA	DC, 1
	DATA	C, 0

	END
