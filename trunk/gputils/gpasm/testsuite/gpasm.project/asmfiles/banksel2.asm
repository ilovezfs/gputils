  processor 16f877a

reg1 equ 0x20
reg2 equ 0xa0
reg3 equ 0x120
reg4 equ 0x1a0

  org 0
  
  banksel reg4
  movwf reg4
  
  banksel reg2
  movwf reg2

  banksel reg3
  movwf reg3

  banksel reg1
  movwf reg1

  end

