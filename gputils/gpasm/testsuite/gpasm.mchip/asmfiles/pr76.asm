; PR76.ASM

; This file tests the processor directive.

	PROCESSOR	16C76  ; 14-bit core

	RETLW	12	; 0812 = 12-BIT
				; 3412 = 14-BIT
				; B612 = 16-BIT

	END
