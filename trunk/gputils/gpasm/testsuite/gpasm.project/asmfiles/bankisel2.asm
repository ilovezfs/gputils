  processor 16f877a

INDF equ H'0000'
FSR  equ H'0004'

reg1 equ 0x20
reg2 equ 0xa0
reg3 equ 0x120
reg4 equ 0x1a0

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

