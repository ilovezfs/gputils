; LIST1.ASM

; This file tests legal formatting of the list file.

	LIST		P=17C44, R=DEC

	TITLE		"This is my list file title."
	SUBTITLE	"This is my list file subtitle."
	PAGE	
	
	SUBTITL		"This is my second subtitle."
	PAGE		

	STITLE		"This is my third subtitle."
	PAGE

; ***********************************
	SPACE
; ***********************************
	SPACE	0
; ***********************************
	SPACE	1
; ***********************************
	SPACE	2
; ***********************************
	SPACE	10
; ***********************************

	PAGE
	NOLIST
; This comment should not appear in the list file!!!
	LIST
; This comment should appear in the list file.

	END
