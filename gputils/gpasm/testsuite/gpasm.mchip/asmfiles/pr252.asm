; PRF252.ASM

; This file tests the processor directive.

        PROCESSOR       18c252  ;32-BIT 

	RETLW	12	; 0812 = 12-BIT
				; 3412 = 14-BIT
				; B612 = 16-BIT
                                ; 0C12 = 32-bit
	END
