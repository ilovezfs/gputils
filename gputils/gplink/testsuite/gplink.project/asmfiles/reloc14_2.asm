	processor 16f877a

	extern	c1, c2
	global	c3, d3, d4

.code3	code 0
	nop
c3	nop

.code4	code 0x800
	pagesel $-1
	pagesel $-1
	pagesel $

.data3	udata 0x30
	res 1
d3	res 1

.data4	udata 0x38
	res 1
d4	res 1

	;; This tests symbol value adjustment when the section is
	;; relocated.
.code5	code
	pagesel c5
c5	goto c5
	dw c5

	end
