; TESTOVR1.ASM
;
; This file tests the udata_shr directive.  It is interspersed with other
; section directives to verify that the sections are started and ended
; properly.

	list	p=16c54

Merge2	udata_ovr
Var5a	res	1

First	udata
Var4	res	1

Merge	udata_ovr
Var1a	res	1
Var1b	res	1

Other	udata
Var3	res	1

Merge	udata_ovr
Var2a	res	1
Var2b	res	1
Var2c	res	1

Merge2	udata_ovr
Var6a	res	2

Code1	code

	clrw
	clrw
	clrw
	clrw

Code2	code

	clrwdt
	clrwdt
	clrwdt
	clrwdt
	clrwdt

Merge2	udata_ovr
Var7a	res	1

Code3	code
	clrf	Var4
	clrf	Var1a
	clrf	Var6a

	end
