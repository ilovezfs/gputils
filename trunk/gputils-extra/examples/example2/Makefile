SCRIPT = 16f873.lkr
OBJECTS =  mastri2c.o init.o i2ccomm.o
DEPDIR = .deps
OUTPUT = example2.hex

all : $(DEPDIR) $(OUTPUT)

$(DEPDIR):
	mkdir -p $(DEPDIR)

$(OUTPUT) : $(OBJECTS) $(SCRIPT)
	gplink --map -s $(SCRIPT) -o $(OUTPUT) $(OBJECTS)

%.o : %.asm
	gpasm -p 16f873 -c -M $< && mv $*.d $(DEPDIR)/$*.d

clean:
	-rm -f *.o *.lst *.map *.hex

distclean: clean
	-rm -Rf $(DEPDIR)

# Dependencies
-include $(DEPDIR)/*.d

