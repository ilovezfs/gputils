; DEFINE1.ASM

; This file tests to make sure one symbol has been defined on the
; command line.  It should be called with the command line:

;       MPASM DEFINE1.ASM /dSYMBOL1

	LIST    P=17C42

	ifdef SYMBOL1
	   MESSG        "Symbol defined correctly"
	else
	   ERROR        "Symbol not defined"
	endif

	end
