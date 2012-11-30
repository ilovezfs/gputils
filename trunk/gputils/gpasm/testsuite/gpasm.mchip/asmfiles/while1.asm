; WHILE1.ASM

; This file tests general use of the WHILE directive.

	LIST	P=17C44, R=DEC

;;;; Begin: Changed in gputils
;;;;	I = 5
I = 5
;;;; End: Changed in gputils
	WHILE I > 0	; Multiple passes
	   DATA	I
;;;; Begin: Changed in gputils
;;;;	   I--
I--
;;;; End: Changed in gputils
	ENDW

;;;; Begin: Changed in gputils
;;;;	I = 10
I = 10
;;;; End: Changed in gputils
	WHILE I == 10	; Single pass
	   DATA	I
;;;; Begin: Changed in gputils
;;;;	   I++
I++
;;;; End: Changed in gputils
	ENDW

;;;; Begin: Changed in gputils
;;;;	I = 10
I = 10
;;;; End: Changed in gputils
	WHILE I == 0	; No pass
	   DATA	I
;;;; Begin: Changed in gputils
;;;;	   I++
I++
;;;; End: Changed in gputils
	ENDW

;;;; Begin: Changed in gputils
;;;;	I = 2
I = 2
;;;; End: Changed in gputils
	WHILE I > 0		; Nested WHILE
;;;; Begin: Changed in gputils
;;;;	   J = 3
J = 3
;;;; End: Changed in gputils
	   WHILE J > 0
	      DATA	I * J
;;;; Begin: Changed in gputils
;;;;	      J--
J--
;;;; End: Changed in gputils
	   ENDW
;;;; Begin: Changed in gputils
;;;;	   I--
I--
;;;; End: Changed in gputils
	ENDW

;SSR 1351, Multiple line macro called from within a WHILE

TEST	MACRO	NUM	
	ADDLW	NUM
	ADDLW	NUM
	ADDLW	NUM
	ENDM

;;;; Begin: Changed in gputils
;;;;	I = 3
I = 3
;;;; End: Changed in gputils
	WHILE I > 0
	   TEST	I
;;;; Begin: Changed in gputils
;;;;	   I--
I--
;;;; End: Changed in gputils
	ENDW

	END
