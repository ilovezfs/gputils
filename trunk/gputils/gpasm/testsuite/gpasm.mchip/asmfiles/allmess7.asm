;ALLMESS7.ASM
;	This file tests the generation of all messages.

	list	p=16c57

	messg	"This is a test message"	; Message 301 MESSAGE:

	addwf	0x85,1				; Message 302 Argument out of range.

	data	0x1000				; Message 303 Program word too large.

	__idlocs 0x12345			; Message 304 ID Locations value too large.

	addwf	1				; Message 305 Using default destination of 1

	goto	0x200				; Message 306 Crossing page boundary...

	lgoto	0x200				; Message 307 Setting page bits

	__MAXRAM	0x1F
	__MAXRAM	0x1F			; Message 310 Superceding current maximum RAM and RAM map.

	movlw	high( 0x12345678 )		; Message 311 Higher order bits of operand are being masked.
	end

