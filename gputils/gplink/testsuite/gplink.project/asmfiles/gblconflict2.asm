; This file contains a global symbol that is also in gblconflict1.asm.  The linker
; should generate and error.

  processor 16f877a

  code
duplabel:
  global duplabel
  movlw 0

  end