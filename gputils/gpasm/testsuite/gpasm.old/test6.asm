;
;
	    PROCESSOR	PIC16C84
	    __CONFIG	0xF3FD
	    CBLOCK 0x0C
	    label1
	    label2
	    ENDC
;				
test		equ	0x00
;
		org	0x00
		goto	start
		org	0x05
start
		movlw	0xff
circle		goto	circle
		end

