; RELOC2.ASM

; This file tests that assembler variables can be used to create
; the offsets for relocation entries.
; It also checks for the TRIS relocation entry, which can't be
; done with the other file and the 17Cxx family.

	list	p=16c54

Offset	= 1

	code

Here

	call	Here+Offset
Offset ++
	call	Here+Offset
Offset ++
	call	Here+Offset
Offset ++
	call	Here+Offset
Offset ++
	call	Here+Offset

	call	Here+(Offset+1)

	call	Offset+2+Here

	extern	TrisDest

	tris	TrisDest

	end
