; MESSAGES.ASM

; This file is used to generate files with messages, as well as
; information suitable for each file type.

	LIST	P=16C54

TOP	NOP

OneReference = 1

	MESSG	"This is a message"
	ERROR	"This is an error"

AnotherReference = 2

ThirdReference = OneReference + AnotherReference

	END
