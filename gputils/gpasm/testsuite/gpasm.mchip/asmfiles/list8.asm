; LIST8.ASM

; This file tests legal usage of the LIST FIXED directive.

; LIST should give warning if in column 1
	list fixed
list	p=16c71

; #DEFINE should not give warnings.
#define Dummy1 1
 #define Dummy2 2

Label1				; OK
Label2	Nop			; OK

;;;; Begin: Changed in gputils
;;;; Label3				; Warning
;;;; Label4	Nop			; Warning
Label3				; Warning
Label4	Nop			; Warning
;;;; End: Changed in gputils

Nop				; Warning
 Nop				; OK
Nep				; OK
;;;; Begin: Removed in gputils
;;;; Nep				; Warning
;;;; End: Removed in gputils

DumbMac	Macro
	nop
	endm

DumbMac				; Warning
 DumbMac			; OK


Setc				; Warning
 Setc				; OK

	end

