; This file contains a many sections.  It along with other memseg files are
; linked.  This tests gplink's relocation algorithm.

  processor 16f877a

; first page
.code_reloc2 code
reloc2:
  global reloc2
  fill 0xff, (0x600 - 0x5)

; lower half of second page
.code_abs2 code 0xd00
abs2:
  global abs2
  fill 0xff, 0xff

; third page
.code_reloc3 code
reloc3:
  global reloc3
  fill 0xff, (0x17ff - 0x1100 - 0xff)

  end
