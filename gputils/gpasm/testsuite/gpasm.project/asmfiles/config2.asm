     list p=16c74,f=inhx8m
     org 0
     movlw 0Xaa

     __config 0x3ffb

     movlw 0x5

     org 0x100

     fill 0x23, 14
     fill 0xff, 58
     fill 0x1534, 0x400
     
     end
