; HD505.ASM

; This file tests header file usage for the specified processor.

        LIST    P=16C505
        INCLUDE "p16c505.inc"



        DATA    _CP_ON, 0X02FF                   
        DATA    _CP_OFF, 0XFFF
        DATA    _WDT_ON, 0XFFF
        DATA    _WDT_OFF, 0XFF7
        DATA    _LP_OSC, 0XFF8
        DATA    _XT_OSC, 0XFF9
        DATA    _HS_OSC, 0XFFA
        DATA    _IntRC_OSC_RB4EN, 0xFFC
        DATA    _IntRC_OSC_CLKOUTEN, 0x0FFD
        DATA    _ExtRC_OSC_RB4EN, 0x0FFE
        DATA    _ExtRC_OSC_CLKOUTEN, 0X0FFF

	


	DATA	PORTB, 6	
        DATA    OSCCAL, 5
	DATA	FSR, 4	
	DATA	STATUS, 3
	DATA	PCL, 2
	DATA	TMR0, 1
	DATA	INDF, 0


;       DATA    RP2, 7
;       DATA    GPWUF, 7
        DATA    PA0, 5
	DATA	NOT_TO, 4
	DATA	NOT_PD, 3
	DATA	Z, 2
	DATA	DC, 1
	DATA	C, 0

	DATA 	T0CS, 5
	DATA	T0SE, 4
	DATA	PSA, 3
	DATA	PS2, 2
	DATA	PS1, 1
	DATA	PS0, 0




	END
