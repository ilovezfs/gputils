; MESSERR.ASM

; This file has illegal uses of the messages.

	LIST	P=16C54

TOP	NOP

OneReference = 1

	MESSG
	ERROR

Label1	MESSG	"This is a message"
Label2	ERROR	"This is an error"

AnotherReference = 2

ThirdReference = OneReference + AnotherReference

	END
