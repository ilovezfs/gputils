; DATA1.ASM

; This file tests general legal uses of the various data directives.

	LIST	P=17C44, R=HEX

CONSTANT	CONST1 = 55
VARIABLE	VAR1 = 66
#DEFINE		DEF1 77

	DATA	1
	DATA	1, 2, 3
	DATA	"ABC"
	DATA	"'EFG'"
	DATA	1, 2, 3, "abc"
	DATA	"Test String\n", 0
	DATA	"\a\b\f\n\r\t\v\\\'\"\?\0"
	DATA	"\000\123\234\377"
	DATA	1111, 2222, 0EEEE, 0FFFF
	DATA	CONST1, VAR1, DEF1
	DATA	$, $, $
	DATA	$, $ - 1, $-2
	DATA	'y'
	DATA	"y"
	DATA	'('
	DATA	')'
	DATA	',', '\\', '\'', '\"', "\""

	DW	1
	DW	1, 2, 3
	DW	"ABC"
	DW	"'EFG'"
	DW	1, 2, 3, "abc"
	DW	"Test String\n", 0
	DW	"\a\b\f\n\r\t\v\\\'\"\?\0"
	DW	"\000\123\234\377"
	DW	1111, 2222, 0EEEE, 0FFFF
	DW	CONST1, VAR1, DEF1
	DW	$, $, $
	DW	$, $ - 1, $-2
	DW  	'y'
	DW  	"y"
	DW  	'('
	DW  	')'
	DW	',', '\\', '\'', '\"', "\""

	DB	1
	DB	1, 2, 3
	DB	"ABC"
	DB	"'EFG'"
	DB	1, 2, 3, "abc"
	DB	"Test String\n", 0
	DB	"\a\b\f\n\r\t\v\\\'\"\?\0"
	DB	"\000\123\234\377"
	DB	11, 22, 0EE, 0FF
	DB	CONST1, VAR1, DEF1
	DB  	'y'
	DB  	"y"
	DB  	'('
	DB  	')'
	DB	',', '\\', '\'', '\"', "\""

	DE	1
	DE	1, 2, 3
	DE	"ABC"
	DE	"'EFG'"
	DE	1, 2, 3, "abc"
	DE	"Test String\n", 0
	DE	"\a\b\f\n\r\t\v\\\'\"\?\0"
	DE	"\000\123\234\377"
	DE	11, 22, 0EE, 0FF
	DE	CONST1, VAR1, DEF1
	DE  	'y'
	DE  	"y"
	DE  	'('
	DE  	')'
	DE	',', '\\', '\'', '\"', "\""

	DT	1
	DT	1, 2, 3
	DT	"ABC"
	DT	"'EFG'"
	DT	1, 2, 3, "abc"
	DT	"Test String\n", 0
	DT	"\a\b\f\n\r\t\v\\\'\"\?\0"
	DT	"\000\123\234\377"
	DT	11, 22, 0EE, 0FF
	DT	CONST1, VAR1, DEF1
	DT  	'y'
	DT  	"y"
	DT  	'('
	DT  	')'
	DT	',', '\\', '\'', '\"', "\""

	END
