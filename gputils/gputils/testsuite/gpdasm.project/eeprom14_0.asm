
	list    p=16f84

Const1	equ	0xCCBBAA
Const2	equ	0x998877
Const3	equ	0x665544
Const4	equ	0x332211

start:
	return

	org	__EEPROM_START

Str0	db	"content0\0"
Data01	db	low(Const1), high(Const1), upper(Const1)
Data02	db	low(Const2), high(Const2), upper(Const2)
Data03	db	low(Const3), high(Const3), upper(Const3)
Data04	db	low(Const4), high(Const4), upper(Const4)

	org	__EEPROM_START + .32

Str1	db	"content1\0"
Data11	db	low(Const1), high(Const1), upper(Const1)
Data12	db	low(Const2), high(Const2), upper(Const2)
Data13	db	low(Const3), high(Const3), upper(Const3)
Data14	db	low(Const4), high(Const4), upper(Const4)

	end
