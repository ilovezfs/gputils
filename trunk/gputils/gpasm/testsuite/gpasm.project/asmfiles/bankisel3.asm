  processor 17c766

INDF0 equ H'0000'
FSR0  equ H'0001'
INDF1 equ H'0008'
FSR1  equ H'0009'

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

  movlw reg4
  movwf FSR1
  bankisel reg4
  movwf INDF1

  movlw reg2
  movwf FSR1  
  bankisel reg2
  movwf INDF1

  movlw reg3
  movwf FSR1
  bankisel reg3
  movwf INDF1

  movlw reg1
  movwf FSR1
  bankisel reg1
  movwf INDF1

  movlw reg8
  movwf FSR0
  bankisel reg8
  movwf INDF0

  movlw reg6
  movwf FSR0
  bankisel reg6
  movwf INDF0

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

