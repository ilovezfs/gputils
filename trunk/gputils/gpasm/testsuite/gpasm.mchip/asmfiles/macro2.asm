; MACRO2.ASM

; This file tests list file represention of expanded macros.

	LIST	P=16C54

#define Def1	1
Value1	EQU	2

Comm	MACRO
	NOP			; This macro has a comment.
	ENDM

Spa	MACRO
	NOP			; This macro has a blank line next.
	
	NOP
	ENDM

Indent	MACRO
	NOP			; No extra indent
	 NOP			; One extra indent
	  NOP			; Two extra indents
	   NOP			; Three extra indents
	ENDM

Subs	MACRO	Arg1, Arg2, Arg3
	DATA	Arg1
	DATA	Arg2
	DATA	Arg3
	ENDM

	Comm
	Spa
	Indent
	Subs	Def1, Value1, 3

	END

