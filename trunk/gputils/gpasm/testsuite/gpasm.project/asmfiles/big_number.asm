; make sure big numbers can be used

  processor 16f877
  radix dec

POSITIVE  EQU 0x9E3779B9
NEGATIVE  EQU -2147483648

  movlw (POSITIVE & 0xFF000000) >> 24
  movlw (NEGATIVE & 0xFF000000) >> 24

  end
