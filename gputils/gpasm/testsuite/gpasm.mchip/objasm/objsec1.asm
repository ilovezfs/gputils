; OBJSEC1.ASM

; This file tests the generic sections of a COFF file.

		list	p=17c44

		extern	Ext1, Ext2, ExtCall

		nolist
E1		equ	1
E2		equ	2
E3		equ	3

		list

		UDATA
U1		res	1
U2		res	1
U3		res	1

Init_data	IDATA	0x10
I1		db	1,2,3

		UDATA_SHR
Shr1		res	1
Shr2		res	1

Temps		UDATA_OVR
T1		res	1
T2		res	1
LongVarName	res	1
	
		CODE

Top
		global	Top

                #include "objsec1.inc"

StartOfProgram
		nop

		end
		
