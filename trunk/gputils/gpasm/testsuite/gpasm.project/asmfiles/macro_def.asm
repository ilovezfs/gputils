; generate an error because macro calls must be after the definition

  processor 16c84

  org 5

  test 5

test macro number1
  movlw number1
  endm
  
  test 5
  
  end
