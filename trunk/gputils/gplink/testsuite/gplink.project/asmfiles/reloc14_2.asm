	processor 16f877a

	extern	c1, c2
	global	c3, d3, d4, d5, code2

.code3	code 0
	nop
c3	nop

.code4	code 0x800
	pagesel $-1
	pagesel $-1
	pagesel $

	banksel d3
	banksel d4
	banksel d5

	bankisel d3
	bankisel d4
	bankisel d5

	dw d5
	dw id1

.data3	udata 0x30
	res 1
d3	res 1

.data4	udata 0xa0
	res 1
d4	res 1

.data5	udata 0x120
	res 1
d5	res 1

	;; This tests symbol value adjustment when the section is
	;; relocated.
.code5	code
	pagesel c5
c5	goto c5
	dw c5

.idata1	idata
	db 1
id1	db $,3
	dw $-1
	dw c1

	;; This tests symbol value adjustment when the section is
	;; merged and relocated.
	code
	pagesel code2
code2	goto code2
	dw code2

	end
