; PR62B.ASM

; This file tests for the processor directive.

        PROCESSOR       16C62B

	RETLW	12		; 0812 = 12-BIT
				; 3412 = 14-BIT
				; B612 = 16-BIT

	END
