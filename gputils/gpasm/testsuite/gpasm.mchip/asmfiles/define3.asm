; DEFINE3.ASM

; This file tests to make sure one symbol has been defined on the
; command line.  It should be called with the command line:

;       MPASM DEFINE3.ASM /dSYMBOL3="abc"

	LIST    P=17C42

	ifdef SYMBOL3
	   MESSG        "Symbol defined correctly"
	else
	   ERROR        "Symbol not defined"
	endif

	DATA    SYMBOL3

	end
