; PREE8.ASM

; This file tests the processor directive.

	PROCESSOR	EEPROM8

	LIST	F=INHX8M

;	RETLW	12		; 0812 = 12-BIT
				; 3412 = 14-BIT
				; B612 = 16-BIT

	DATA	12, 34

	END
