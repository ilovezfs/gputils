  processor 18f8720

FSR0  equ 0
FSR1  equ 1
FSR2  equ 2
INDF0 equ H'0FEF'
INDF1 equ H'0FE7'
INDF2 equ H'0FDF'

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

  movlw reg4
  movwf FSR0
  bankisel reg4
  movwf INDF0

  movlw reg2
  movwf FSR0
  bankisel reg2
  movwf INDF0

  movlw reg3
  movwf FSR1
  bankisel reg3
  movwf INDF1

  movlw reg1
  movwf FSR1
  bankisel reg1
  movwf INDF1

  movlw reg8
  movwf FSR2
  bankisel reg8
  movwf INDF2

  movlw reg6
  movwf FSR2
  bankisel reg6
  movwf INDF2

  movlw reg7
  movwf FSR0
  bankisel reg7
  movwf INDF0

  movlw reg5
  movwf FSR0
  bankisel reg5
  movwf INDF0

  movlw reg9
  movwf FSR0
  bankisel reg9
  movwf INDF0

  end

