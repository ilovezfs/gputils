; This file contains a many sections.  It along with other memseg files are
; linked.  This tests gplink's relocation algorithm.

  processor 16f877a

; third page
.code_reloc6 code
reloc6:
  global reloc6
  fill 0x66, (0x10ff - 0x1000 + 1)

; start of fourth page
.code_abs4 code 0x1800
abs4:
  global abs4
  fill 0x04, (0x18ff - 0x1800 + 1)

  end
