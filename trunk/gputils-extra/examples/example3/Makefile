# Makefile for example

EXECUTABLE = main.hex
OBJECTS = main.o delay.o stack.o

all: $(EXECUTABLE)

%.o : %.pal
	gpal -c p16f877.pub $(COMMON) $<

$(EXECUTABLE): $(OBJECTS)
	gplink -m -c -s 16f877.lkr -o $(EXECUTABLE) $(OBJECTS)

clean:
	rm -f *.cod *.hex *.o *.map *.asm *.cof *.lst

COMMON = config.pub types.pub delay.pub stack.pub main.pub 

# Dependencies
main.o :  main.pal $(COMMON)
delay.o : delay.pal $(COMMON)
stack.o : stack.pal $(COMMON)
