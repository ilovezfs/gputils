; LIST3.ASM

; This file tests legal usage of the LIST directive.

	LIST	P=17c42

	DATA	1, 2, 3, 4, 5, 6, 7

Mac	MACRO
	NOP
			; Comment line, next line blank

	ENDM

	LIST	WRAP
; This is a big long line that will need to be wrapped in the list file based on the default column width, which is 132.
	LIST	NOWRAP
; This is a big long line that will be truncated in the list file based on the default column width, which is 132.
	LIST	WRAP

	LIST	B=4
; No tabs
	; One tab
		; Two tabs
	LIST	B=6
; No tabs
	; One tab
		; Two tabs
	LIST	B=8
; No tabs
	; One tab
		; Two tabs

	LIST	C=40
; This line will have to be wrapped.
; This line will have to be wrapped twice because of the new column width.
	LIST	C = 132

	LIST	F = INHX32	; Check generated hex file for format

	PAGE
	LIST	L=10		; Play with page length, 6 needed for header
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
	LIST	N = 15
	Mac
	Mac
	Mac
	LIST	N = 60

	LIST	R=H
	DATA	10
	LIST	R=D
	DATA	10
	LIST	R=O
	DATA	10
	LIST	R=HEX
	DATA	10
	LIST	R=DEC
	DATA	10
	LIST	R=OCT
	DATA	10
	LIST	R=HEXADECIMAL
	DATA	10
	LIST	R=DECIMAL
	DATA	10
	LIST	R=OCTAL
	DATA	10
	LIST	R=HEX

	LIST	X=ON
	Mac
	LIST	X=OFF
	Mac
	LIST	X=ON

	LIST	X=OFF,C=40,R=DEC
	Mac
; Here's a big long line for wrapping to show column width.
	DATA	10

	LIST	X = ON , C = 50 , R = HEX
	Mac
; Here's a big long line for wrapping to show column width.
	DATA	10

	LIST	C=80

; Wrap data
	DATA	1
	DATA	1, 2
	DATA	1, 2, 3
	DATA	1, 2, 3, 4
	DATA	1, 2, 3, 4, 5
	DATA	1, 2, 3, 4, 5, 6
	DATA	1, 2, 3, 4, 5, 6, 7

MakDat	MACRO	Arg
	DATA	Arg
	ENDM

	MakDat	"a"
	MakDat	"ab"
	MakDat	"abc"
	MakDat	"abcd"
	MakDat	"abcde"
	MakDat	"abcdef"
	MakDat	"abcdefg"
	MakDat	"abcdefgh"
	MakDat	"abcdefghi"
	MakDat	"abcdefghij"
	MakDat	"abcdefghijk"
	MakDat	"abcdefghijkl"
	MakDat	"abcdefghijklm"
	MakDat	"abcdefghijklmn"

	LIST	C=50
;Wrap both
	DATA	1, 2, 3, 4, 5, 6, 7
	MakDat	"abcdefghijklmn"

	END
	
