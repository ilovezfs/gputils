; ALLMESS9.ASM

; This file tests for errors and messages when generating an object file.

	list	p=16c58a

Label1				; Error 150 Labels must be defined...

	org	0		; Error 147 Directive not allowed

	NOP			; Error 152 Executable code and data must...

Code1	CODE

	MOVLW	Label1 << 2	; Error 151 Operand contains unresolvable...

	PAGESEL	Label1 << 2	; Error 151 Operand contains unresolvable... (was 153)

Code1	CODE			; Error 154 Each object file section...

Over1	UDATA_OVR	0

Over1	UDATA_OVR	0x10	; Error 155 All overlayed sections...

	END
