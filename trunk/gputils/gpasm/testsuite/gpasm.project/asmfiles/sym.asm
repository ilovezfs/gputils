        list P=PIC18F4550, F=INHX32

#define bmRequestType 0x01

inrtgt  macro   a, b
a#v(b)
        endm

        end
