; HDE674.ASM

; This file tests header file usage for the specified processor.
; 1/8/97    Commented out the VCLMP options.  These have been removed from the
;           header file.

        LIST    P=12CE674
        INCLUDE "p12c672.inc"



        DATA _MCLRE_ON,       H'3FFF'
        DATA _MCLRE_OFF,      H'3F7F'
        DATA _CP_ALL,         H'009F'
        DATA _CP_75,          H'15BF'
        DATA _CP_50,          H'2ADF'
        DATA _CP_OFF,         H'3FFF'
        DATA _PWRTE_OFF,      H'3FFF'
        DATA _PWRTE_ON,       H'3FEF'
        DATA _WDT_ON,         H'3FFF'
        DATA _WDT_OFF,        H'3FF7'
        DATA _LP_OSC,         H'3FF8'
        DATA _XT_OSC,         H'3FF9'
        DATA _HS_OSC,         H'3FFA'
        DATA _INTRC_OSC,      H'3FFC'
        DATA _INTRC_OSC_NOCLKOUT,   H'3FFC'
        DATA _INTRC_OSC_CLKOUT,     H'3FFD'
        DATA _EXTRC_OSC,            H'3FFE'
        DATA _EXTRC_OSC_NOCLKOUT,   H'3FFE'
        DATA _EXTRC_OSC_CLKOUT,     H'3FFF'
	


	DATA	GPIO, 6	
	DATA	OSCCAL, 5
	DATA	FSR, 4	
	DATA	STATUS, 3
	DATA	PCL, 2
	DATA	TMR0, 1
	DATA	INDF, 0


;	DATA	PA2, 7
;       DATA    GPWUF, 7
;	DATA	PA1, 6
;       DATA    PA0, 5
	DATA	NOT_TO, 4
	DATA	NOT_PD, 3
	DATA	Z, 2
	DATA	DC, 1
	DATA	C, 0

;       DATA    NOT_GPWU, 7
	DATA	NOT_GPPU, 6
	DATA 	T0CS, 5
	DATA	T0SE, 4
	DATA	PSA, 3
	DATA	PS2, 2
	DATA	PS1, 1
	DATA	PS0, 0




	END
