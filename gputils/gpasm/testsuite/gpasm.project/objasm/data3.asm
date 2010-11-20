     list p=18c242
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
     db 0xFF, 0xFF
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

	;; TODO test that odd numbers give Error[159]  : Cannot use FILL directive with odd number of bytes
     fill 0x1ff, 4
cfill1
     fill (bra cfill1), 4
cfill2
     fill (bra $), 4
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
     db 0xFF, 0xFF
     db file
idb

     de "My Program, v1.0",0
     de 1,2,"a",4,"bc",5,"de",6,7
     de file
ide

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
;     fill 0x1ff, 4
;ifill1
;     fill (bra ifill1), 4
;ifill2
;     fill (bra $), 4
;ifill3

     res 1
ires1
     res 2
ires2
     res 3
ires3
     res 4
ires4

packed code_pack
     da "abcdefg"
     da "abcdefg",0
     da "12345678",0
     da 1,2,"a",4,"bc",5,"de",6,7
     da 0xFFFF
     da file
pda

     data "abcdefg"
     data "12345678",0x1421,2,3,4
     data 1,2,"a",4,"bc",5,"de",6,7
     data 0xFFFF
     data file
pdata

     db 't',0x0f,'e',0x0f,'s',0x0f,'t','\n'
     db 1,2,"a",4,"bc",5,"de",6,7
     db 0xFF, 0xFF
     db file
pdb

     de "My Program, v1.0",0
     de 1,2,"a",4,"bc",5,"de",6,7
     de file
pde

     dw "abcdefg"
     dw "12345678",0
     dw 1,2,"a",4,"bc",5,"de",6,7
     dw 0xFFFF
     dw file
pdw

     dt "A Message",0
     dt file
pdt

     fill 0x1ff, 4
pfill1
     fill (bra pfill1), 4
pfill2
     fill (bra $), 4
pfill3

	;; We were left at odd address and res would give error 158
     db 1
	;; Warning[231]: No memory has been reserved by this instruction
     res 1
pres1
	;; This should reserve 0xFFFF
     res 2
pres2
     res 3
pres3
     res 4
pres4

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
     udata_acs
ua0
     res 1
ua1  res 2
ua3  res 3
ua6  res 4
ua10
     end
