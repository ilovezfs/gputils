; RAW1.ASM

; This file test raw data contents of the COFF file.

        LIST    P=17C44

        IDATA
Bytes   DB      1, 2, 3
Words   DW      H'1234', H'5678'
String  DB      "Hi", 0
Final	DB	0xFF

	CODE
Bytes2  DB      1, 2, 3
Words2  DW      H'1234', H'5678'
String2 DB      "Hi", 0
Final2	DB	0xFF

        END
