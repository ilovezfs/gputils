; UNDEF2.ASM

; This file tests the illegal usage of the UNDEFINE directive.

	LIST	P=17C42

undefine			; Just a label
#undefine			; No parameters
#undefine Def1			; Symbol not defined

	END
