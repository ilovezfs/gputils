	processor 18f2420

	extern	c1, c2
	global	c3, d3, d4

.code3	code 0
	nop
c3	nop
	BC	c1
	BN	c1
	BNC	c1
	BNN	c1
	BNOV	c1
	BNZ	c1
	BOV	c1
	BRA	c1
	BZ	c1
	RCALL	c1

	;; I know, this is nonsense on pic16e
.code4	code 0x800
	pagesel $-1
	pagesel $-1
	pagesel $

.data3	udata_acs 0x20
	res 1
d3	res 1

.data4	udata 0x200
	res 1
d4	res 1

	end
