; LIST7.ASM

; This file tests illegal usage of the LIST directive.

	LIST	P=Stupid

Mac	MACRO
	NOP
			; Comment line, next line blank
	NOP
	ENDM

	LIST	WRAP=ON
; This is a big long line that will need to be wrapped in the list file based on the default column width, which is 132.
	LIST	NOWRAPPING
; This is a big long line that will be truncated in the list file based on the default column width, which is 132.
	LIST	WRAP

	LIST	B4
; No tabs
	; One tab
		; Two tabs
	LIST	B
; No tabs
	; One tab
		; Two tabs

	LIST	C
; This line will have to be wrapped.
; This line will have to be wrapped twice because of the new column width.
	LIST	C =
	LIST	C=0

	PAGE
	LIST	L
	LIST	L=
	LIST	L=1		; Play with page length, 6 needed for header
; Line 1
; Line 2
; Line 3
; Line 4
; Line 5
; Line 6
; Line 7
; Line 8
; Line 9
; Line 10
; Line 11
; Line 12
; Line 13
;	LIST	N = 15
;	Mac
;	Mac
;	Mac
	LIST	N = 60

	LIST	R
	LIST	R=
	LIST	R=HE
	DATA	10
	LIST	R=DECI
	DATA	10
	LIST	R=OO
	DATA	10
	LIST	R=HEX

	LIST	X
	LIST	X=
	LIST	X=O
	Mac
	LIST	X=ONN
	Mac
	LIST	X=OF

	LIST	F=ABC

	END
	
