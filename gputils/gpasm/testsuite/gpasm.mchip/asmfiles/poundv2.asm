; POUNDV2.ASM

; This file tests illegal usage of #V substitution

        list    p=16c71,r=dec

I = 1
;Label#v			; actually, this is a legal label
Label#v(			; no expression
Label#v(I			; non-terminated expression
#v(I)				; illegal label
#v(I)Label			; illegal label

	end
