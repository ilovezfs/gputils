; PRE673.ASM

; This file tests the processor directive.

        PROCESSOR       12CE673  ; 14-bit core

	RETLW	12	; 0812 = 12-BIT
				; 3412 = 14-BIT
				; B612 = 16-BIT

	END
