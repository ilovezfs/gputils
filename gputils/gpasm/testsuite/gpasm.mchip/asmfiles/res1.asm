; RES1.ASM

; This file tests legal usage of the RES directive.

	LIST	P=16C54

	ORG	0
	RES	10
	DATA	111

	RES	0
	DATA	222

	RES	1
	DATA	333

Label	RES	20
	DATA	444

	END
