; FORMATS.ASM

; This test file is to test the generation of the different types of hex
; files.  Since it must be valid for all, it can only generate data in the
; first 64K bytes (32K words) of memory. 

	LIST	P=17C42

	DATA	1, 2, 3, "ABC"

	END
