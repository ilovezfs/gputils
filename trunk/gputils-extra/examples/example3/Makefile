# Makefile for example

OBJECTS = main.o delay.o stack.o

all: example.hex

%.o : %.pal
	gpal -t -c $<

example.hex: $(OBJECTS)
	gpal -k"-m -c -s 16f877.lkr" -o example.hex $(OBJECTS)
        
clean:
	rm -f *.cod *.hex *.o *.map *.asm *.cof *.lst

COMMON = config.pub types.pub

main.o : main.pub main.pal $(COMMON)
delay.o : delay.pub delay.pal $(COMMON)
stack.o : stack.pub stack.pal $(COMMON)
