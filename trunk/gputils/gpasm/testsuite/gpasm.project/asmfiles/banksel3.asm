  processor 17c766

reg1 equ 0x010
reg2 equ 0x110
reg3 equ 0x210
reg4 equ 0x310
reg5 equ 0x410
reg6 equ 0x510
reg7 equ 0x610
reg8 equ 0x710
reg9 equ 0x810

  org 0
  
  banksel reg4
  movwf reg4
  
  banksel reg2
  movwf reg2

  banksel reg3
  movwf reg3

  banksel reg1
  movwf reg1

  banksel reg8
  movwf reg8
  
  banksel reg6
  movwf reg6

  banksel reg7
  movwf reg7

  banksel reg5
  movwf reg5

  banksel reg9
  movwf reg9

  end

