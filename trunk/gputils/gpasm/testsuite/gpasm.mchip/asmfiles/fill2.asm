; FILL2.ASM

; This file tests legal usage of the FILL directive.

	LIST	P=16C54, R=HEX

ROMSize = 0x200

Start	ORG	0
	FILL	(GOTO Start), ROMSize

	END
