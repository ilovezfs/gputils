; IFDEF1.ASM

; This file tests legal usage of the IFDEF and IFNDEF directives.

	LIST	P=17C42

#define	Def1

	IFDEF	Def1
	   DATA	"OK"
	ENDIF

	IFDEF	Def2
	   ERROR	"Symbol not defined"
	ENDIF

	IFNDEF	Def1
	   ERROR	"Symbol not defined"
	ENDIF

	IFNDEF	Def2
	   DATA	"OK"
	ENDIF

	IFDEF	Def1
	   DATA	"OK"
	ELSE
	   ERROR	"Symbol was defined"
	ENDIF

	IFDEF	Def2
	   Error	"Symbol was not defined"
	ELSE
	   DATA	"OK"
	ENDIF

	IFNDEF	Def1
	   ERROR	"Symbol was defined"
	ELSE
	   DATA	"OK"
	ENDIF

	IFNDEF	Def2
	   DATA	"OK"
	ELSE
	   Error	"Symbol was not defined"
	ENDIF

	END
