; HD58B.ASM

; This file tests header file usage for the specified processor.

	LIST	P=16C58B
	INCLUDE	"p16c5x.inc"

 	DATA	_CP_ON,	0X0007		; This changes per device

	DATA	STATUS, 3
	DATA	PORTA, 5
	DATA	PORTB, 6

	DATA	Z, 2
	DATA	DC, 1
	DATA	C, 0

	DATA 	T0CS, 5
	DATA	T0SE, 4
	DATA	PSA, 3

	END
