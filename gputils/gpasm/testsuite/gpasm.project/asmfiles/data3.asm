     list p=18c242,f=inhx32 
     include "p18c242.inc" 

test equ 0x15

     __idlocs _IDLOC0, 0x12 
     __idlocs _IDLOC1, 0xff 
     __idlocs _IDLOC2, 99 
     __idlocs _IDLOC3, 66
     __idlocs _IDLOC4, test 
     __idlocs _IDLOC5, 0xAA 
     __idlocs _IDLOC6, 0xF 
     __idlocs _IDLOC7, 0xA

     da "abcdefg"
     da "12345678",0
     da 0xFFFF

     data "abcdefg"
     data "12345678",0x1421,2,3,4
     data 0xFFFF

     db 't',0x0f,'e',0x0f,'s',0x0f,'t','\n'

     de "My Program, v1.0",0

     dw "abcdefg"
     dw "12345678",0
     dw 0xFFFF

     dt "A Message",0

     end
