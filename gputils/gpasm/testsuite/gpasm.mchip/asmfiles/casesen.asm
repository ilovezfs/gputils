; CASESEN.ASM

; This file is used to illustrate the differences between case 
; sensitivity and insensitivity.  It should be assembled once
; without command line options and once with /c.

	LIST    P=16C54

OneLabel = 10                                   ; mixed case
ONELABEL = 20                                   ; upper case

	if OneLabel == 20                       ; if value changed
	   MESSG        "Case Insensitive"
	else
	   MESSG        "Case Sensitive"
	endif

	end
