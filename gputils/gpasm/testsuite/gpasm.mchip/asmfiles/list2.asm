; LIST2.ASM

; This file tests illegal formatting of the list file.

	LIST		P=17C44, R=DEC

	TITL		"This is my list file title."
	STITL		"This is my list file subtitle."
	PAGE	2	
	
Label1	TITLE		"This is my second title."
	SUBTITLE	"This is my second subtitle."
	PAGE	

; ***********************************
	SPACE	-1
; ***********************************
	SPACE	a
; ***********************************

	PAGE
	NOLIST	1
; Comment...
	LIST	1
; Comment...
Label3	NOLIST	
; Comment...
Label4	LIST	
; Comment...


	END
