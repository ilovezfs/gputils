; WHILE1.ASM

; This file tests general use of the WHILE directive.

	LIST	P=17C44, R=DEC

	I = 5
	WHILE I > 0	; Multiple passes
	   DATA	I
	   I--
	ENDW

	I = 10
	WHILE I == 10	; Single pass
	   DATA	I
	   I++
	ENDW

	I = 10
	WHILE I == 0	; No pass
	   DATA	I
	   I++
	ENDW

	I = 2
	WHILE I > 0		; Nested WHILE
	   J = 3
	   WHILE J > 0
	      DATA	I * J
	      J--
	   ENDW
	   I--
	ENDW

;SSR 1351, Multiple line macro called from within a WHILE

TEST	MACRO	NUM	
	ADDLW	NUM
	ADDLW	NUM
	ADDLW	NUM
	ENDM

	I = 3
	WHILE I > 0
	   TEST	I
	   I--
	ENDW

	END
