; PR923.ASM

; This file tests the processor directive.

        PROCESSOR       16C923

	RETLW	12		; 0812 = 12-BIT
				; 3412 = 14-BIT
				; B612 = 16-BIT

	END
