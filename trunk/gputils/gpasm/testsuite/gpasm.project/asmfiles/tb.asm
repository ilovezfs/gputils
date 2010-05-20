	list p=16f73

	;; This is about a bug in MPASM 5.34 where this org is shifted
	;; left by one and produces byte offset -1. That is seen as too
	;; large, but it is not seen to wrap around and hence gives no
	;; error.
	org 0x7FFFFFFF
	movlw 0x12
	movlw 0x34
	end
