;  This file is used to test relocatable objects with the sx instruction 
;  set.  Some of the expressions are possible, but not very useful.

     list p=sx18

     extern file, address

     code     
     addwf file,1 
     andlw file
     andwf file,1
     bcf file,7
     bsf file,7
     btfsc file,1
     btfss file,1
     call file
     clrf file
     clrw
     clrwdt
     comf file,1
     decf file,1
     decfsz file,1
     goto address
     incf file,1
     incfsz file,1
     iorlw file
     iorwf file,1
     movf  file,1
     movlw file
     movwf file
     nop
     option 
     retlw file
     rlf file,1
     rrf file,1
     sleep
     subwf file,1
     swapf file,1
     tris file
     xorlw file
     xorwf file,1
     ; sx only
     bank file
     iread
     mode file
     movmw
     movwm
     page file
     reti
     retiw
     retp
     return
     end
