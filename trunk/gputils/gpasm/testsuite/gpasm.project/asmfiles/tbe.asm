	list p=16f73

	org 0xFFFF
	movlw 0x12
	movlw 0x34 		; Error 117 Address wrapped around 0
	end
