; DEFINE4.ASM

; This file tests to make sure one symbol has been defined on the
; command line.  It should be called with the command line:

;       MPASM DEFINE4.ASM /dSYMBOL1 /dSYMBOL2=123 /dSYMBOL3="abc"

	LIST    P=17C42

	ifdef SYMBOL1
	   MESSG        "SYMBOL1 defined correctly"
	else
	   ERROR        "SYMBOL1 not defined"
	endif

	ifdef SYMBOL2
	   MESSG        "SYMBOL2 defined correctly"
	else
	   ERROR        "SYMBOL2 not defined"
	endif

	ifdef SYMBOL3
	   MESSG        "SYMBOL3 defined correctly"
	else
	   ERROR        "SYMBOL3 not defined"
	endif

        DATA    SYMBOL2
        DATA    SYMBOL3
	
	end
