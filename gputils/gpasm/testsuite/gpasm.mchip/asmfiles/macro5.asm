; MACRO5.ASM

; This file tests using EXITM from within a macro.

	LIST	P=16C54

Mac1	MACRO
	NOP
	EXITM
	ENDM

Mac2	MACRO	Num
	IF Num > 5
	   EXITM
	ELSE
	   NOP
	ENDIF
	ENDM

Mac3	MACRO	Num
	WHILE Num > 0
Num--
	   EXITM
	ENDW
	ENDM

	Mac1
	Mac2	3
	Mac2	8
OtherNum = 0
	Mac3	OtherNum
OtherNum = 2
	Mac3	OtherNum

	END
