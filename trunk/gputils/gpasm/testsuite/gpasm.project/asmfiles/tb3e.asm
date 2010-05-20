	list p=18c242

	;; MPASM 5.34 and does not catch these wrap arounds
	;; 21 bit counters for external memory
	org 0x1FFFFE
	movlw 0x12
	movlw 0x34
	;; PCLATU overflow
	org 0xFFFFFE
	movlw 0x12
	movlw 0x34
	end
