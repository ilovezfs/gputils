;ERRLVL1.ASM
;   This file tests the enabling/disabling of a specific message

	list	p=17c42

	errorlevel	1, -201
	radix	dec


	messg	"This shouldn't appear anywhere"

; This should generate a suppressed warning
#undefine NotDefined

;This should generate a non-suppressed warning
	data	"Hi there


	errorlevel	+201

;This should generate a non-suppressed warning
#undefine NotDefined

;This should generate a non-suppressed warning
	data	"Hi there

;This should generate a warning - errors cannot be suppressed
	errorlevel	-101

	end
