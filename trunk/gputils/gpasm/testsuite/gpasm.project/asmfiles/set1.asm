; This file tests for the proper evaluation of set in a macro

  processor 16c84

test macro 
number set number + 1
  retlw number
  endm

number set 0
  test
  test
  test

  end

