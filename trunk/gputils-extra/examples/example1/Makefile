# Example1

# system programs
RM              = /bin/rm -f
ARC             = tar -cvf
ZIP             = gzip

# gpasm programs
ASM             = gpasm
DIASM           = gpdasm
CODVIEWER       = gpvc -d 

# project data
PROJECT         = example1
VERSION         = 0.0.1
MAIN_FILE       = rs232.asm 
FILE_LIST       = rcvr.asm rs232.h txmtr.asm

# derived variables
ARCHIVE         = $(PROJECT)_$(VERSION).tar
HEX_FILE        = $(PROJECT).hex
COD_FILE        = $(PROJECT).cod

all:	$(HEX_FILE)

$(COD_FILE): $(HEX_FILE)

$(HEX_FILE): $(MAIN_FILE) $(FILE_LIST)	
	$(ASM) -o $(HEX_FILE) $(MAIN_FILE)

diasm: $(HEX_FILE)
	$(DIASM) $(HEX_FILE)

viewcod: $(COD_FILE)
	$(CODVIEWER) $(COD_FILE)

$(ARCHIVE):
	$(ARC) $(ARCHIVE) *

archive: $(ARCHIVE)
	$(ZIP) $(ARCHIVE)

clean:
	$(RM) *.hex *.lst *.cod $(PROJECT)*.gz 

