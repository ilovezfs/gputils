;LABELS2.ASM
;	This file tests for illegal labels.

	list	p=16c54,r=dec

ThisLabelIsTooLong_ByOneCharacter	equ	1
Illegal%Character			equ	2

	end
