; PR50.ASM

; This file tests illegal use of the processor directive.

	PROCESSOR		; No value
	PROCESSOR	16C50	; Illegal value

	RETLW	12		; 0812 = 12-BIT
				; 3412 = 14-BIT
				; B612 = 16-BIT

	END
