; This file tests for an error caused by the defines table not being
; cleared after the first pass.

   processor 16f877

   org 0x0000 

   goto START 

#IFNDEF temp
#DEFINE temp 

FUNC 
    nop 
    nop 
    return 

#ENDIF 

START 
   nop 
   nop 
   call FUNC 
   goto START 
   end
