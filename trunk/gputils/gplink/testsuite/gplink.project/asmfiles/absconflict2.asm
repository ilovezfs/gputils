; This file contains an absolute section that overlaps with a section in 
; absconflict1.asm.  The linker should generate and error.

  processor 16f877a

.code_2 code 0x110
  fill 0xff, 0xff

  end
