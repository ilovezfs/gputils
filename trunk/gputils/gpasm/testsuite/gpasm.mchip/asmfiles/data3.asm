; DATA3.ASM

; This file tests illegal uses of the various data directives.

	LIST	P=17C44, R=DEC

	DATA	-32769
	DATA	65536
	DATA	"abc

	DW	-32769
	DW	65536
	DW	"abc

	DB	-129
	DB	256
	DB	"abc

	DE	-129
	DE	256
	DE	"abc

	DT	-129
	DT	256
	DT	"abc

	END
