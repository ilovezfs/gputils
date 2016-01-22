     list p=12f1822
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

     fill 0x1ff, 2
cfill1
     fill (goto cfill1), 2
cfill2
     fill (goto $), 2
cfill3

     res 1
cres1
     res 2
cres2
     res 3
cres3
     res 4
cres4

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

	;; MPASM gives Error[152]  : Executable code and data must be defined in an appropriate section 
     ;de "My Program, v1.0",0
     ;de 1,2,"a",4,"bc",5,"de",6,7
     ;de file

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
;     fill 0x1ff, 2
;ifill1
;     fill (goto ifill1), 2
;ifill2
;     fill (goto $), 2
;ifill3

     udata
u0
     res 1
u1   res 2
u3   res 3
u6   res 4
u10
     udata_ovr
uo0
     res 1
uo1  res 2
uo3  res 3
uo6  res 4
uo10
     udata_shr
us0
     res 1
us1  res 2
us3  res 3
us6  res 4
us10
     end
