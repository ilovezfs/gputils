; numbers not assigned a value are evaluated as 0 in IF statements 

  processor 16c84

test macro
  local novalue

  if novalue
    error "novalue evaluated"
  else
    movlw 1
  endif
  endm

  test


  end
