SCRIPT = 16f873.lkr
OBJECTS =  mastri2c.o init.o i2ccomm.o
OUTPUT = example2.hex

all : $(OUTPUT)

$(OUTPUT) : $(OBJECTS) $(SCRIPT)
	gplink --map -s $(SCRIPT) -o $(OUTPUT) $(OBJECTS)

%.o : %.asm
	gpasm -p 16f873 -c $<

clean:
	rm -f *.o *.lst *.map *.hex

mastri2c.o : mastri2c.asm mastri2c.inc i2ccomm1.inc flags.inc
init.o : init.asm
i2ccomm.o : i2ccomm.asm i2ccomm.inc i2ccomm1.inc flags.inc


