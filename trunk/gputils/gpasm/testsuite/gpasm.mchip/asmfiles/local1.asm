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
	MyVar = 1
	WHILE MyVar <= Param1
	   DATA	MyVar
	   MyVar++
	ENDW
	ENDM


	Mac1
	Mac2	1
	Mac2	6

	MyVar = 2
	DATA	MyVar
	Mac3	4
	DATA	MyVar	

;***** Test for SSR 1756

Mac4	macro
	local	Var = 3
	while Var > 0
	   Mac5	Var
	   Var--
	endw
	endm

Mac5	macro	Value
	data	Value
	endm

	Mac4

;***** End Test

	END
