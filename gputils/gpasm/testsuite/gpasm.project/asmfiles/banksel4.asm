  processor 18f8720

reg1 equ 0x000
reg2 equ 0x060
reg3 equ 0x100
reg4 equ 0x200
reg5 equ 0x300
reg6 equ 0x400
reg7 equ 0x600
reg8 equ 0xd00
reg9 equ 0xfff

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

