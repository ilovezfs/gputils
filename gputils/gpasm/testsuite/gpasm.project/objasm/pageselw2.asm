  list p=16f73
  #include "p16f73.inc"

vars udata
tmp res 1

tbl_dat_sec code
tbl_dat:
  retlw 0x1
  retlw 0x2
  retlw 0x3
  retlw 0x4

tbl_lookup_sec code
tbl_lookup:
  pageselw tbl_dat
  movf tmp, w
  addlw LOW tbl_dat
  skpnc
  incf PCLATH, f
  movwf PCL

main_sec code
main:
  pagesel tbl_lookup
  movlw 0x1
  call tbl_lookup
  movlw 0x3
  call tbl_lookup

  end
