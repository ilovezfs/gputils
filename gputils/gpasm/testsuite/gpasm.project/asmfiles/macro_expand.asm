; This file tests macro expansion.

       list p=16c84

; define the macros used in the test

#include "macro.inc"

nest   macro
       movlw 0xf
       endm

; nested macros
test1  macro
       local label2
       movlw 2
label2
       movlw 3
       ; this is a comment
       nest
       movlw 4
       movlw 5
       endm

; while loop within a macro
test2  macro
       movlw 0
j = 1
       while (j<5) 
         movlw j
         movlw j+1
j++
       endw
       movlw 0
       endm

; start the test

begin
       expand
       
       movlw 1
       ; test expanded nested macros
label1 test1
       movlw 0
       ; test expand macro with while loop inside
       test2
       movlw 0
       ; test expanded nested macros that reside in different source files
       test3

       movlw 0
       noexpand

       movlw 1
       ; test nonexpanded nested macros
label3 test1
       movlw 0
       ; test expand macro with while loop inside
       test2
       movlw 0
       ; test nonexpanded nested macros that reside in different source files
       test3

; verify that EXPAND and NOEXPAND don't effect while loops

       expand

i = 0
       while (i<5) 
         movlw i
         movlw i+1
i++
       endw

       movlw 0

       noexpand

i = 0
       while (i<5) 
         movlw i
         movlw i+1
i++
       endw

       expand

       goto begin

       end
