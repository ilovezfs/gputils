; MACRO1.ASM

; This file tests how global labels can be generated from macros.

	LIST	P=16C54

MakLab	MACRO	Num
Label#v(Num)
	DATA	Num
	ENDM

;;;; Begin: Changed in gputils
;;;;	I = 4
I = 4
;;;; End: Changed in gputils
	WHILE I > 0
	   MakLab	I
;;;; Begin: Changed in gputils
;;;;	   I--
I--
;;;; End: Changed in gputils
	ENDW

;;;; Begin: Changed in gputils
;;;;	I = 4
I = 4
;;;; End: Changed in gputils
	WHILE I > 0
	   GOTO	Label#V(I)
;;;; Begin: Changed in gputils
;;;;	   I--
I--
;;;; End: Changed in gputils
	ENDW

	GOTO	Label1
	GOTO	Label2
	GOTO	Label3
	GOTO	Label4

	END

