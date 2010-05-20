     list p=17c42
     extern file

     code
     da "abcdefg"
     da "abcdefg",0
     da "12345678",0
     da 1,2,"a",4,"bc",5,"de",6,7
     da 0xFFFF
     da file
cda

     data "abcdefg"
     data "12345678",0x1421,2,3,4
     data 1,2,"a",4,"bc",5,"de",6,7
     data 0xFFFF
     data file
cdata

     db 't',0x0f,'e',0x0f,'s',0x0f,'t','\n'
     db 1,2,"a",4,"bc",5,"de",6,7
     db file
cdb

     de "My Program, v1.0",0
     de 1,2,"a",4,"bc",5,"de",6,7
     de file
cde

     dw "abcdefg"
     dw "12345678",0
     dw 1,2,"a",4,"bc",5,"de",6,7
     dw 0xFFFF
     dw file
cdw

     dt "A Message",0
     dt file
cdt

     fill 0x1ff, 3
cfill1
     fill (goto cfill1), 3
cfill2
     fill (goto $), 3
cfill3

     res 1
cres1
     res 2
cres2
     res 3
cres3

     idata
     da "abcdefg"
     da "abcdefg",0
     da "12345678",0
     da 1,2,"a",4,"bc",5,"de",6,7
     da 0xFFFF
     da file
ida

     data "abcdefg"
     data "12345678",0x1421,2,3,4
     data 1,2,"a",4,"bc",5,"de",6,7
     data 0xFFFF
     data file
iddata

     db 't',0x0f,'e',0x0f,'s',0x0f,'t','\n'
     db 1,2,"a",4,"bc",5,"de",6,7
     db file
idb

     ;de "My Program, v1.0",0
     ;de 1,2,"a",4,"bc",5,"de",6,7
     ;de file
;ide

     dw "abcdefg"
     dw "12345678",0
     dw 1,2,"a",4,"bc",5,"de",6,7
     dw 0xFFFF
     dw file
idw

	;; MPASM gives Error[152]  : Executable code and data must be defined in an appropriate section 
     ;dt "A Message",0
     ;dt file

	;; MPASM gives Error[152]  : Executable code and data must be defined in an appropriate section 
;     fill 0x1ff, 3
;ifill1
;     fill (goto ifill1), 3
;ifill2
;     fill (goto $), 3
;ifill3

     res 1
ires1
     res 2
ires2
     res 3
ires3

     end
