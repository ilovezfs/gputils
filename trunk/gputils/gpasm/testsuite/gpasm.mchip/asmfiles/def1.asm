; DEF1.ASM

; This file test legal usage of DEFINE.

	LIST    P=16C71, R=HEX

#DEFINE STATUS  3
CARRY_BIT = 0

#DEFINE Defined2
#DEFINE Number2 123
#DEFINE String2 "ABC"
#DEFINE Carry2  STATUS,CARRY_BIT
#DEFINE Yloc2( X , M , B )           ( M * X + B )
#DEFINE MyJump2( Addr )              GOTO  Addr
#DEFINE LoPrec2( A , B )             ( A + B )

IFNDEF Defined2
   ERROR "Symbol Not Defined"
ENDIF

	DATA    Number2
	DT	String2
	BTFSC   Carry2
	DATA    Yloc2 ( 2, 5, 3 )       ; 0D
	MyJump2 ( Bottom )
	DATA    LoPrec2 ( 1 , 2 ) * 3   ; 9

Bottom

#DEFINE Subtract(v)	addlw -(v)

	Subtract	(1)		; Using parens
	Subtract	2		; Not using parens
	Subtract	3
	Subtract	1+3
	Subtract	-2
	MyJump2		Bottom2

Bottom2

;***** Test for SSR 1618

#DEFINE Nest(A, B)  A + B
Nest1a EQU Nest(1, Nest(2, 3))
Nest1b EQU Nest( Nest( 5, 6 ), Nest(7,8) )
Nest1c EQU Nest( Nest( Nest(1,2), Nest(3,4) ), Nest( Nest(5,6), Nest(7,8)) ) 
	DATA	Nest1a, Nest1b, Nest1c

;***** End Test

;***** Test for SSR 2143

#define MACRO	NOP

	MACRO		; Should generate a NOP instruction

;***** End Test

	END
