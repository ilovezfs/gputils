; IF3.ASM 

; This file tests illegal usage of the IF-ELSE-ENDIF directives.

	LIST    P=17C42

;***** Test for SSR #2566

	Variable I = 0
	WHILE I <= 5
	   IF I == 0
	      NOP
	   END				; END instead of ENDIF
	   I++
	ENDW

	END
;***** End test

