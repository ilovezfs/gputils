;ALLMESS6.ASM
;	This file generates several warnings.  It must be assembled with the command line
;	or interface options:
;		Processor: 	16c71
;		Radix:		Decimal
;		Hex Output:	INHX32

	list	p=16c61, r=hex, f=inhx8m	; Warning 215 Processor superceded by command line
						; Warning 216 Radix superceded by command line
						; Warning 217 Hex file format specified on command line
	data	10
	end
