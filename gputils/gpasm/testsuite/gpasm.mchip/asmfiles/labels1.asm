;LABELS1.ASM
;	This file tests valid label characters and lengths

	list	p=16c54, r=dec

a					equ	1	; One character label, lower case
A					equ	2	; One character label, upper case, NO CONFLICT
MixedCase				equ	3	; Mixed case label
WithColon:				equ	4	; Trailing colon

Number42				equ	5	; Numeric included
Number43:				equ	6	; Trailing colon

Here_Now				equ	7	; Special character included
DoThis?					equ	8	; Special character included
Now?:					equ	9	; Trailing colon

_special				equ	10	; Leading special
?What					equ	11	; Leading special
_OK:					equ	12	; Trailing colon

?HereIsA_BIG_MishMash?:			equ	13	; Mishmash
Führer					equ	14	; Character above 128 ASCII

ThisIsABigLongLabelOfMaxLength32	equ	15	; Max length label

	end

