  processor 16c84
  
  
nest1 macro string, number1
  messg string
  movlw  number1
  endm
  
  
nest2 macro number
  nest1 "Craig Franklin", number
  endm
  
test macro input
  nest2 input
  endm
  
  test 3
  
  end
