; CONFIG4.ASM

; This file tests the __CONFIG directive for 16-bit core with illegal 
; output format.

	LIST	P=17C44, R=HEX		; , F=INHX32

	__CONFIG	1234

	END
