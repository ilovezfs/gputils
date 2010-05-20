     list p=eeprom16
     extern file

     code

     data "abcdefg"
     data "12345678",0x1421,2,3,4
     data 1,2,"a",4,"bc",5,"de",6,7
     data 0xFFFF
     data file
cdata

     end
