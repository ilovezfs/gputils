; HD461.ASM

; This file tests header file usage for the specified processor.
; 1/8/97    Commented out the VCLMP options.  These have been removed from the
;           header file.

        LIST    P=16C461
        INCLUDE "p16c461.inc"



; 	DATA	_VCLMP_ON,  0X0FFF
; 	DATA	_VCLMP_OFF, 0X0FDF
 	DATA	_MCLRE_ON,  0X0FFF
 	DATA	_MCLRE_OFF, 0X0FEF
        DATA    _CP_ALL,     0X0FF7                       
	DATA	_CP_OFF,    0X0FFF
	DATA	_WDT_ON,    0X0FFF
	DATA	_WDT_OFF,   0X0FFB
	DATA	_LP_OSC,    0X0FFC
	DATA	_XT_OSC,    0X0FFD
        DATA    _INTRC_OSC, 0X0FFE
        DATA    _EXTRC_OSC, 0X0FFF
	


        DATA    PORTA, 6 
	DATA	OSCCAL, 5
	DATA	FSR, 4	
	DATA	STATUS, 3
	DATA	PCL, 2
	DATA	TMR0, 1
	DATA	INDF, 0


;	DATA	PA2, 7
;       DATA    GPWUF, 7
;       DATA    RP1, 6
        DATA    RP0, 5
	DATA	NOT_TO, 4
	DATA	NOT_PD, 3
	DATA	Z, 2
	DATA	DC, 1
	DATA	C, 0

;       DATA    NOT_GPWU, 7
;       DATA    NOT_GPPU, 6
	DATA 	T0CS, 5
	DATA	T0SE, 4
	DATA	PSA, 3
	DATA	PS2, 2
	DATA	PS1, 1
	DATA	PS0, 0




	END
