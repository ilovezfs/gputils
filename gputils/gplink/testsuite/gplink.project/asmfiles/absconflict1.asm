; This file contains an absolute section that overlaps with a section in 
; absconflict2.asm.  The linker should generate and error.

  processor 16f877a

.code_1 code 0x100
  fill 0xff, 0xff

  end
