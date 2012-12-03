; LOCAL1.ASM

; This file tests legal usage of the LOCAL and MACRO-EXITM-ENDM
; directives.

	LIST	P=16C54

Mac1	MACRO
	NOP
	ENDM

Mac2	MACRO	Param1
	IF Param1 > 5
	   EXITM
	ELSE
	   DATA	Param1
	ENDIF
	ENDM

Mac3	MACRO	Param1
	LOCAL	MyVar
;;;; Begin: Changed in gputils
;;;;	MyVar = 1
MyVar = 1
;;;; End: Changed in gputils
	WHILE MyVar <= Param1
	   DATA	MyVar
;;;; Begin: Changed in gputils
;;;;	   MyVar++
MyVar++
;;;; End: Changed in gputils
	ENDW
	ENDM


	Mac1
	Mac2	1
	Mac2	6

;;;; Begin: Changed in gputils
;;;;	MyVar = 2
MyVar = 2
;;;; End: Changed in gputils
	DATA	MyVar
	Mac3	4
	DATA	MyVar	

;***** Test for SSR 1756

Mac4	macro
	local	Var = 3
	while Var > 0
	   Mac5	Var
;;;; Begin: Changed in gputils
;;;;	   Var--
Var--
;;;; End: Changed in gputils
	endw
	endm

Mac5	macro	Value
	data	Value
	endm

	Mac4

;***** End Test

	END
