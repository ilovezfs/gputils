; This file contains a many sections.  It along with other memseg files are
; linked.  This tests gplink's relocation algorithm.

  processor 16f877a

; second page
.code_reloc4 code
reloc4:
  global reloc4
  fill 0xff, (0xd00 - 0x800)

; second page
.code_reloc5 code
reloc5:
  global reloc5
  fill 0xff, (0x17ff - 0x1100 - 0xff - 1)

; fourth page
.code_reloc1 code
reloc1:
  global reloc1
  fill 0xff, (0x1fff - 0x1800 - 0xff - 1)

; upper half of third page
.code_abs3 code 0x1100
abs3:
  global abs3
  fill 0xff, 0xff

; end of first page
.code_abs1 code 0x600
abs1:
  global abs1
  fill 0xff, 0x1ff

  end
