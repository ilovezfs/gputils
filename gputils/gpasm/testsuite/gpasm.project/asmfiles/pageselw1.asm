  list p=16f876a
  #include "p16f876a.inc"

  cblock 0x20
    tmp
  endc

	org 0x005
tbl_dat:
  retlw 0x1
  retlw 0x2
  retlw 0x3
  retlw 0x4

	org 0x020
tbl_lookup:
  pageselw tbl_dat
  movf tmp, w
  addlw LOW tbl_dat
  skpnc
  incf PCLATH, f
  movwf PCL

	org 0x800
main:
  pagesel tbl_lookup
  movlw 0x1
  call tbl_lookup
  movlw 0x3
  call tbl_lookup

  end
