; PRF658.ASM

; This file tests the processor directive.

        PROCESSOR       18c658  ;32-BIT 

	RETLW	12	; 0812 = 12-BIT
				; 3412 = 14-BIT
				; B612 = 16-BIT
                                ; 0C12 = 32-bit
	END
