; ORG2.ASM

; This file tests illegal usage of the ORG directive.

	LIST	P=16C54, R=HEX

	ORG			; No value
	DATA	$

Label	ORG	
	DATA	Label		; No value

	END
