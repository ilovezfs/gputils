; This file contains a many sections.  It along with other memseg files are
; linked.  This tests gplink's relocation algorithm.

  processor 16f877a

; first page
.code_reloc2 code
reloc2:
  global reloc2
  fill 0x22, (0x5ff + 1)

; lower half of second page
.code_abs2 code 0xd00
abs2:
  global abs2
  fill 0x02, (0xdff - 0xd00 + 1)

; third page
.code_reloc3 code
reloc3:
  global reloc3
  fill 0x33, (0xfff - 0xe00 +1)

  end
