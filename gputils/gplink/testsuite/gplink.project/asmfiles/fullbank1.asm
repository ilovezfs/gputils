; This file creates four blocks of file registers that use 
; all the available general purpose file registers

  processor 16f877a

.udata_fullbank0 udata
one_big_array0 res 0x4f

.udata_fullbank1 udata
one_big_array1 res 0x4f

.udata_fullbank2 udata
one_big_array2 res 0x5f

.udata_fullbank3 udata
one_big_array3 res 0x5f

; This code ensures that something will be in the hex file
; that has the addresses of the data above.

.code_full_bank code
  movlw 0

  banksel one_big_array0
  movwf one_big_array0

  banksel one_big_array1
  movwf one_big_array1 + 0x10

  banksel one_big_array2
  movwf one_big_array2 + 0x20

  banksel one_big_array3
  movwf one_big_array3 + 0x30
  
  end
