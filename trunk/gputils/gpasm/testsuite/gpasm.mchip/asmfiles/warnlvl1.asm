; WARNLVL1.ASM

; This file is used to test warning level setting in the list file.  It
; uses both the LIST directive and the ERRORLEVEL directive.  Note that
; the command line will override either of these.  Since warnings cannot
; be explicitly generated, we will use a ROM specification to generate a
; warning.

	LIST	P=16C54
	ORG	0x200


	MESSG	"A Message"
	NOP
	ERROR	"An Error"

	LIST	W=0
	MESSG	"A Message"
	NOP
	ERROR	"An Error"

	LIST	W=1
	MESSG	"A Message"
	NOP
	ERROR	"An Error"

	LIST	W=2
	MESSG	"A Message"
	NOP
	ERROR	"An Error"

	ERRORLEVEL	0
	MESSG	"A Message"
	NOP
	ERROR	"An Error"

	ERRORLEVEL	1
	MESSG	"A Message"
	NOP
	ERROR	"An Error"

	ERRORLEVEL	2
	MESSG	"A Message"
	NOP
	ERROR	"An Error"

	END

	
