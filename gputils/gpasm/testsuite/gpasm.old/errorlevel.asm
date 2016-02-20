; Author:  Craig Franklin
; Date:    11-12-00
;
; This code was used to test changes made to GPASM 0.8.11 to implement the ERRORLEVEL
; directive and several other enhancements to the ERROR handling system.

; ON or OFF is valid
	list p=16c84, st=fred;

; Bad radix setting
        list r=yhr
        radix yhr

	include "p16c84.inc"

; Check for divide by zero error;
test1 equ 12/0
test2 equ 12/12

; New decimal notation clean up
test3 equ .128
test4 equ 0x80

; Check the error level setting using ERRORLEVEL DIRECTIVE
     errorlevel 0, -.226, -.302 
     messg "Hello"    ;message
     errorlevel -.123  ;warning

     errorlevel 1
     messg "Hello"    ;message
     errorlevel -.123  ;warning

     errorlevel 2
     messg "Hello"    ;message
     errorlevel -.123  ;warning

; Check the error level setting using LIST DIRECTIVE
     list w=0 
     messg "Hello"    ;message
     errorlevel -.123  ;warning

     list w=1
     messg "Hello"    ;message
     errorlevel -.123  ;warning

     list w=2
     messg "Hello"    ;message
     errorlevel -.123  ;warning

     list w=0

; Invalid error levels
     list w=3
     errorlevel 3

; Suppress messages	
     messg "Hello"    ;Print message
     errorlevel -.301 ;Suppress message 301
     messg "Hello"    ;Make sure message doesn't print
     errorlevel +.301 ;Enable message 301
     messg "Hello"    ;Print message

; redefine processor
     list p=16f84

; Missing arguments
     messg

; Extraneous arguments
     nop test1

; Illegal argument
     list test

; Argument out of range
     bcf 0x20,8

; Invalid Ram location
     movf 0xffff,0

; Missing w or f
     movf 0x20

; Illegal Conditions
     endif
     else
     endw
     endm

; Finish
     end
