; This file tests the substitution of labels

  list p=16c84

#define oldlabel newlabel

  movlw 1
  movlw 2
  movlw 3
  movlw 4

oldlabel movlw 5

  movlw 6
  movlw 7
  movlw 8

  goto oldlabel
  goto newlabel

#undefine oldlabel

  movlw 9

  goto newlabel

  end

