; This file contains a many sections.  It along with other memseg files are
; linked.  This tests gplink's relocation algorithm.

  processor 16f877a

; second page
.code_reloc4 code
reloc4:
  global reloc4
  fill 0x44, (0xcff - 0x800 + 1)

; second page
.code_reloc5 code
reloc5:
  global reloc5
  fill 0x55, (0x17ff - 0x1200 + 1)

; fourth page
.code_reloc1 code
reloc1:
  global reloc1
  fill 0x11, (0x1fff - 0x1900 + 1)

; upper half of third page
.code_abs3 code 0x1100
abs3:
  global abs3
  fill 0x03, (0x11ff - 0x1100 + 1)

; end of first page
.code_abs1 code 0x600
abs1:
  global abs1
  fill 0x01, (0x7ff - 0x600 + 1)

  end
