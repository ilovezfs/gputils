  processor 16c84

  list n=0

constant1 equ 5

test1 macro number1, number2, number3, number4 
  movlw number1
  fill (movlw number2), number3
  movlw number4
  endm

test2 macro string
  messg string
  endm

test3 macro string
string equ 5
  endm

test4 macro number, value
label_#v(number) equ value
  endm

test5 macro number
  if (number==1)
    exitm
  else
    movlw 0
  endif
  movlw 0
  endm

  #include "test.inc"

  test1 1, 2, 3, constant1

  test1 b'1', o'2', h'3', constant1

  test2 "hello"

  test3 myvar1
  movlw myvar1
  
  test4 3, 4
  movlw label_3

  test5 0

  test5 1

  movlw 5

  end


