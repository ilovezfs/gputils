; FUSES4.ASM

; This file tests the __FUSES directive for 16-bit core with illegal 
; output format.

	LIST	P=17C44, R=HEX		; , F=INHX32

	__FUSES	1234

	END
