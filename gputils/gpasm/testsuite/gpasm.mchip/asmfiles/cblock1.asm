; CBLOCK1.ASM

; This file tests legal usage of CBLOCK.

	LIST    P=16C54,R=HEX

	CBLOCK                  ; Start at 0.
		Block01
	ENDC
	
	CBLOCK                  ; Start where left off - 1
		Block02
	ENDC

	CBLOCK  0x10            ; Start at 0x10
		Block03
	ENDC

	CBLOCK  0x20            ; Multiple symbols on one line
		Block04, Block05, Block06
	ENDC

	CBLOCK  0x30            ; Multiple lines
		Block07
		Block08
		Block09
	ENDC

	CBLOCK  0x40            ; General mishmash
		Block10,Block11
		Block12
		Block13  ,  Block14  ,  Block15
	ENDC

	DATA    Block01, Block02, Block03
	DATA    Block04, Block05, Block06
	DATA    Block07, Block08, Block09
	DATA    Block10, Block11, Block12
	DATA    Block13, Block14, Block15

	CBLOCK	0x0a
		w
	ENDC

	DATA	w

;***** Test for SSR #2070

#define QUEUE_SIZE	20

	CBLOCK	0x100
	   TwoByte:0, TwoByteHigh, TwoByteLow
	   QueueBuffer : QUEUE_SIZE
	   QueueHead, QueueTail
	   CalcSize : (2+3) * 2
	   EndMark
	ENDC

	DATA	TwoByte, TwoByteHigh, TwoByteLow
	DATA	QueueBuffer
	DATA	QueueHead, QueueTail
	DATA	CalcSize
	DATA	EndMark

;***** End test

	END


