  processor 16c58b

reg1 equ 0x10
reg2 equ 0x30
reg3 equ 0x50
reg4 equ 0x70

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

