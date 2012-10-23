; POUNDV1.ASM

; This file tests legal usage of #V substitution

        list    p=16c71,r=dec

#define d1 5
;;;; Begin: Changed in gputils
;;;;        v1 = 1
v1 = 1
;;;; End: Changed in gputils

mult#v(1)and#v(2)  ; col 20

padmac      macro  label, num, instruction, dat
label#v(num)       instruction dat
label#v(num)a      instruction dat
            endm

            padmac AA, 123, ADDLW, 1

            padmac AAAA, 234, ADDLW, v1

            padmac AAAAAA, 345, ADDLW, d1

#define lab2  myfunnylabel1
#define lab2a myfunnylabel2

funny   macro vv
lab#v(vv)     ; col 15
;;;; Begin: Changed in gputils
;;;;        vv = vv + 1
vv = vv + 1
;;;; End: Changed in gputils
lab#v(vv)     ; col 15 myfunnylabel1  may not be substituted correctly
lab#v(vv)a    ; col 15 myfunnylabel2  may not be substituted correctly
        endm

;;;; Begin: Changed in gputils
;;;;        var = 1
var = 1
;;;; End: Changed in gputils
        funny var


test    macro
        local   i
;;;; Begin: Changed in gputils
;;;;        i=0
i=0
;;;; End: Changed in gputils
        while i < 3
addr#v(i)  goto addr#v(i)a   ; col 30
addr#v(i)a data addr#v(i)    ; col 30
;;;; Begin: Changed in gputils
;;;;           i++
i++
;;;; End: Changed in gputils
        endw
        endm

        test

;;;; Begin: Changed in gputils
;;;;        v2 = 1
v2 = 1
;;;; End: Changed in gputils

label#v(v1)        ; col 20
v1++
	data	label1

label#V( v1 )      ; col 20
v1++
	data	label2

label#V( (v1) )    ; col 20
v1++
	data	label3

label#v(v1 + v2)   ; col 20
v1++
	data	label5

label#v((v1+v2)*2) ; col 20
v1++
	data	label12

#define d2 d1 + 1

label#v(v1+d1)
v1++
	data	label11

label#v(v1+d2)
v1++
	data	label13

label#v(v1)nopad
v1++
	data	label8nopad

label#v(v1)nopad   ; col 20 (may not be maintained)
v1++
	data	label9nopad

label#v(v1) ;pad to 13
v1++
	data	label10

v=12345678
label#v(v);col 11, sub longer that orig

v=12345678
label#v(v) ;col 12, sub longer that orig

v=12345678
label#v(v)  ;col 13, sub longer that orig

v=12345678
label#v(v)   ;col 14, sub longer that orig

v=12345678
label#v(v)    ;col 15, sub longer that orig

v=12345678
label#v(v)     ;col 16, sub longer that orig

v=12345678
label#v(v)      ;col 17, sub longer that orig

v=12345678
label#v(v)       ;col 18, sub longer that orig

v=12345678
label#v(v)        ;col 19, sub longer that orig
	data	label12345678


mymac       macro  label, num
label#v(num)       data   num
            endm

            mymac  maclab, 1
	data	maclab1
            mymac  maclab, v1
	data	maclab11
            mymac  maclab, d1
	data	maclab5
            mymac  maclab, d2
	data	maclab6
            mymac  maclab, d1 + d2 + 1
	data	maclab12

    end
