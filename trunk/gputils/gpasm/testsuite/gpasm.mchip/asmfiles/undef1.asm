; UNDEF1.ASM

; This file tests the legal usage of the UNDEFINE directive.

	LIST	P=17C42

#define	Def2	1	; Define
	DATA	Def2
#undefine Def2
	IFDEF Def2
	   ERROR	"Symbol should not be defined"
	ELSE
	   DATA	"OK"
	ENDIF 

#define	Def2	2	; Redefine
	DATA	Def2
#undefine Def2
	IFDEF Def2
	   ERROR	"Symbol should not be defined"
	ELSE
	   DATA	"OK"
	ENDIF 

	END
