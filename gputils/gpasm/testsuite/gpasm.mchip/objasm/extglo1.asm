; EXTGLO1.ASM

; This file tests the legal usage of the External and Global directives.
; It must be assembled with the /o option.

        LIST    P=16C71

        EXTERN         A1
        EXTERN         B1,C1
        EXTERN         D1 , E1


	UDATA
A2	RES		1
B2	RES		1


	CODE
C2	RES		1
D2	RES		1
E2	RES		1

	MOVWF		A1
	CALL		B1
	GOTO		D1

		
        GLOBAL         A2
        GLOBAL         B2,C2
        GLOBAL         D2 , E2

        END
