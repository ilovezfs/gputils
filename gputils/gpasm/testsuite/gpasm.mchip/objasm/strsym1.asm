; STRSYM1.ASM

; This file checks the symbol table and string table generation.

		list	p=17c44

UDATA1		UDATA

Short1		res	1
Short2		res	1
Short3		res	1

UDATA2		UDATA

Short4		res	1
Short5		res	1
Short6		res	1

UDATA3		UDATA

LongVarName1	res	1
LongVarName2	res	1
LongVarName3	res	1

CODE1		CODE

Addr1		nop
Addr2		nop
Addr3		nop

LongAddrName1	nop
LongAddrName2	nop
LongAddrName3	nop

CODE2		CODE

Addr4		nop
Addr5		nop
Addr6		nop

LongAddrName4	nop
LongAddrName5	nop
LongAddrName6	nop

		nolist
		nop
		list

		extern	ExtVar
GlobAdd
		global	GlobAdd

		end
