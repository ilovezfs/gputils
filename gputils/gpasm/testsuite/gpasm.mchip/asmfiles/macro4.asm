; MACRO4.ASM

; This file tests parameter substitution in macros.

	LIST	P=17C42

#define Def1	10
#define Def2 	"ABC"
#define Def3	Var1

Var1	= 1
	VARIABLE Var2 = 2
	CONSTANT Const1 = 3

MakDat	MACRO	Arg1, Arg2, Arg3
	DATA	Arg1
	DATA	Arg2
	DATA	Arg3
	ENDM

Make1	MACRO 	Arg
	DATA	Arg
	ENDM

	MakDat	1, 2, 3
	MakDat	Def1, Def2, Def3
	MakDat	Var1, Var2, Const1
	MakDat	',', ",", '\''
	MakDat	"\'", '\"', "\""

	Make1	','
	Make1 	","
	Make1	'\''
	Make1	"\'"
	Make1	'\"'
	Make1	"\""
	Make1	'\\'
	Make1	"\\"

	Make1	"A \'big\' test."
	Make1	"A \"big\" test."

	END
