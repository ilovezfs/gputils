; ORG1.ASM

; This file tests legal usage of the ORG directive.

	LIST	P=16C54, R=HEX

	ORG	0
	DATA	$

	ORG	10
	DATA	$

Label	ORG	20
	DATA	Label		; Should be 20

	END
