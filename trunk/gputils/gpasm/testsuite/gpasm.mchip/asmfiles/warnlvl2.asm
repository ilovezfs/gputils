; WARNLVL2.ASM

; This file is used to test errors in warning level setting in the list file.  It
; uses both the LIST directive and the ERRORLEVEL directive.  Note that
; the command line will override either of these.  Since warnings cannot
; be explicitly generated, we will use a ROM specification to generate a
; warning.

	LIST	P=16C54

	LIST	W
	MESSG	"A Message"
	NOP
	ERROR	"An Error"

	LIST	W=
	MESSG	"A Message"
	NOP
	ERROR	"An Error"

	LIST	W=3
	MESSG	"A Message"
	NOP
	ERROR	"An Error"

	ERRORLEVEL
	MESSG	"A Message"
	NOP
	ERROR	"An Error"

	ERRORLEVEL	a
	MESSG	"A Message"
	NOP
	ERROR	"An Error"

	ERRORLEVEL	3
	MESSG	"A Message"
	NOP
	ERROR	"An Error"

	END

	
