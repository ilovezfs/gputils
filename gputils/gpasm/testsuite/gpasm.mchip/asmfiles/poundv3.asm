; SSR 3354

; Complex #v substitutions not working, escpecially through macro calls.

	list	p=17c44,r=dec

#define Lab0(x)		LabA#v(x)
#define Val1		0
#define Lab1		LabB#v(Val1)
#define Lab2(x)		LabC#v(x)z#v(Val1)

Lab0(123)				; Makes label named LabA123	= 0
Lab1					; Makes label named LabB0	= 0
Lab2(7)					; Makes label named LabC7z0	= 0

;*******************************************************************************

Sub1	macro		Arg1, Value
#define Arg1		Value
	endm

	Sub1		Name1, 5	; Makes define Name1		= 5

;*******************************************************************************

Sub2	macro		Arg1, Value
Arg1#v(Value)		equ	Value
	endm

	Sub2		Name2, 7	; Makes label Name27		= 7

;*******************************************************************************

#define Sub3Val1		9
#define Sub3Val2		7
#define Sub3Val3		5
#define Sub3Sub(x,y)	x#v(y)#v(Sub3Val1)

Sub3	macro		Name, Value
Sub3Sub(Name,Sub3Val2)	set	Value
Name#v(Sub3Val3)	set	0
	endm

Sub3Sub(OutMac,3)	set	2	; Makes label OutMac39		= 2
	Sub3		InMac, 3	; Makes label InMac79		= 3
					;   and label InMac5		= 0

;*******************************************************************************

Sub4	macro		Name, Value
#define	Name		Name#v(Value)
	endm

	Sub4		NumVar, 1	; Makes define NumVar		= NumVar#v(1)

;*******************************************************************************

	end

