	processor 12c509

	extern	c1, c2
	global	c3, d3, d4, p6, code2

.data06	udata 0x16
p6	res 1

.code3	code 0x200
	pagesel $-1
	pagesel $-1
	pagesel $
c3	nop

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

.data5	udata
	res 1
d5	res 1
	res 2
	res 2

	;; This tests symbol value adjustment when the section is
	;; merged and relocated.
	code
	pagesel code2
code2	goto code2
	dw code2

	end
