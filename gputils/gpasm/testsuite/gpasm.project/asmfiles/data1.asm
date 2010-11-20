     list p=12c508
     __idlocs h'1234' 

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

     res 1
     db 1
     res 2
     db 2
     res 3
     db 3
     res 4
     db 4

     end
