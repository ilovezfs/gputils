;  This file is used to test relocatable objects with the 14 bit instruction 
;  set.  Some of the expressions are possible, but not very useful.

     list p=16f877

     extern file, address

     code
     addlw file
     addwf file,1 
     andlw file
     andwf file,1
     bcf file,7
     bsf file,7
     btfsc file,1
     btfss file,1
     call address
     clrf file
     clrw
     clrwdt
     comf file,1
     decf file,1
     decfsz file,1
     pagesel address
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
     retfie
     retlw file
     return
     rlf file,1
     rrf file,1
     sleep
     sublw file
     subwf file,1
     swapf file,1
     tris file
     xorlw file
     xorwf file,1
     end
