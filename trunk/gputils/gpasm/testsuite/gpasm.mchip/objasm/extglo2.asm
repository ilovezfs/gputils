; EXTGLO2.ASM

; This file tests the illegal usage of the Extern and Global directives.
; It must be assembled with the /o option.

        LIST    P=16C71

	EXTERN
Label1	EXTERN		A1

	GLOBAL
	GLOBAL		NotDef
Label2	GLOBAL		I2

        END
