  processor 16c58b

INDF equ  H'0000'
FSR  equ  H'0004'

reg1 equ 0x10
reg2 equ 0x30
reg3 equ 0x50
reg4 equ 0x70

  org 0
  
  movlw reg4
  movwf FSR
  bankisel reg4
  movwf INDF

  movlw reg2
  movwf FSR
  bankisel reg2
  movwf INDF

  movlw reg3
  movwf FSR
  bankisel reg3
  movwf INDF

  movlw reg1
  movwf FSR
  bankisel reg1
  movwf INDF

  end
