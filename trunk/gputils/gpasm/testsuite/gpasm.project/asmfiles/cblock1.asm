  processor 16c84

number1 equ 6


  cblock 0x20
    sara
    my_var_#v(number1 / 2) : (3 * 4)  
    my_var_#v(number1 + 2)
  
  endc
  
  org 0
  
  movwf my_var_#v(number1 / 2)
  movwf my_var_#v(number1 + 2)
 
  end
