;ALLMESS1.ASM
;     This file generates as many errors as it can.

	processor p16c40	; Error 132 Unknown processor

	list	p=16c57

	error   User error	; Error 101 ERROR: ...

#include "missing.inc"		; Error 105 Cannot open...

#define a1 a2
#define a2 a3
#define a3 a4
#define a4 a5
#define a5 a6
#define a6 a7
#define a7 a8
#define a8 a9
#define a9 1
	data	a1		; Error 106 String sub...

	data	h'12q'		; Error 107 Illegal digit

Lab~el				; Error 108 Illegal character

	data	(1+2		; Error 109 Unmatched (

	data	1+2)		; Error 110 Unmatched )

	equ	1		; Error 111 Missing symbol

	data	1 2		; Error 112 Missing operator

	data	stupid		; Error 113 Symbol not previously defined

	data	4/0		; Error 114 Divide by 0

DupLabel equ 1
DupLabel equ 2			; Error 115 Duplicate label

DupAdd	nop
DupAdd				; Error 116 Address label duplicated...

	org	10
	nop
	org	10
	nop			; Error 118 Overwritting...

	call	0x100		; Error 120 Call or jump not allowed at this address

Dummy   if 0 == 0		; Error 121 Illegal label
	endif

Label	stupid			; Error 122 Illegal opcode

	list	stupid		; Error 124 Illegal argument

	endif			; Error 125 Illegal condition

	tris	10		; Error 126 Argument out of range

DMac	macro
	nop
	endm
	DMac 	dummy		; Error 127 Too many arguments

	andlw			; Error 128 Missing arguments

	list	x=dumb		; Error 129 Expected...

	processor	16c71	; Error 130 Processor type previously...

	list	f=inhx		; Error 134 Illegal hex...

	list	f=inhx32
	org	0ffff
	nop
	nop			; Error 117 Address wrapped...
	org	20

	macro			; Error 135 Macro name missing
	endm

mac1	macro
	mac2
	endm
mac2	macro
	mac3
	endm
mac3	macro
	mac4
	endm
mac4	macro
	mac5
	endm
mac5	macro
	mac6
	endm
mac6	macro
	mac7
	endm
mac7	macro
	mac8
	endm
mac8	macro
	mac9
	endm
mac9	macro
	mac10
	endm
mac10	macro
	mac11
	endm
mac11	macro
	mac12
	endm
mac12	macro
	mac13
	endm
mac13	macro
	mac14
	endm
mac14	macro
	mac15
	endm
mac15	macro
	mac16
	endm
mac16	macro
	mac17
	endm
mac17	macro
	nop
	endm

mac17	macro			; Error 136 Duplicate macro...
	nop
	endm

	mac1			; Error 137 Macros nested...

#include "allmes1a.inc"		; Error 138 Include files nested...

i = 1
j = 0
        while i > 0		; Error 139 Maximum of... 
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
j++
i--
	endw

i=200
	while i > 0
i--
	endw			; Error 140 WHILE must terminate...

i = 1
	while i
	while i
	while i
	while i
	while i
	while i
	while i
	while i
	while i			; Error 141 WHILEs nested too deep
i = 0
	endw
	endw
	endw
	endw
	endw
	endw
	endw
	endw
	endw

i = 1
	if i
	if i
	if i
	if i
	if i
	if i
	if i
	if i
	if i
	if i
	if i
	if i
	if i
	if i
	if i
	if i
	if i			; Error 142 IFs nested too deep
	   nop
i = 0
	endif
	endif
	endif
	endif
	endif
	endif
	endif
	endif
	endif
	endif
	endif
	endif
	endif
	endif
	endif
	endif
	endif

;i = 1
;j = 1
;	if i
;	   while i
;	else			; Error 143 Illegal nesting
;	   while j
;	endif
;i = 0
;j = 0
;	   endw

	endc			; Error 144 Unmatched ENDC

	endm			; Error 145 Unmatched ENDM

	exitm			; Error 146 Unmatched EXITM

#define x BigLongLabelName
BigLongLabelName = 1		; Error 148 Expanded source line exceeded ...
TooLong = x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x

	extern 	ExternVar	; Error 149 Directive only allowed when generating an object file


	end
