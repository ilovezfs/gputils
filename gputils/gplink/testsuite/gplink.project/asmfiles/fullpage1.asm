; This file creates four blocks of data in program memory that 
; use all the available memory.

  processor 16f877a

.code_fullpage0 code
  fill 0xff, 0x7fa

.code_fullpage1 code
  fill 0xaa, 0x7ff

.code_fullpage2 code
  fill 0x55, 0x7ff

.code_fullpage3 code
  fill 0x0f, 0x7ff

  
  end
