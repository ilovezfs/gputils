; FILL1.ASM

; This file tests legal usage of the FILL directive.

	LIST	P=16C54, R=HEX

Value1	= 456
Size1	= 8

Start
	FILL	123, 5
	FILL	Value1, Size1
	FILL	Value1 - 6, Size1 - 2

;***** Test for SSR 2089 **************************************************
	FILL	$, 5
;***** End Test ***********************************************************

;***** Test for SSR 3117 **************************************************
	FILL	(BCF 3, 4), 5
;***** End Test ***********************************************************


	FILL	(GOTO Start), NextBlock-$

NextBlock	ORG	0x100
	END
