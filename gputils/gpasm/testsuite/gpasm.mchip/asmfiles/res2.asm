; RES2.ASM

; This file tests illegal usage of the RES directive.

	LIST	P=16C54

	ORG	0
	RES			; No parameter
	DATA	111

	END
