; DEFINE2.ASM

; This file tests to make sure one symbol has been defined on the
; command line.  It should be called with the command line:

;       MPASM DEFINE2.ASM /dSYMBOL2=123

	LIST    P=17C42

	ifdef SYMBOL2
	   MESSG        "Symbol defined correctly"
	else
	   ERROR        "Symbol not defined"
	endif

	DATA    SYMBOL2

	end
