; IF2.ASM 

; This file tests illegal usage of the IF-ELSE-ENDIF directives.

	LIST    P=17C42

#define Def1    5
Value1  EQU     10

	IF                              ; No expression
	   NOP
	ENDIF

	IF 1=1                          ; Illegal condition
	   NOP
	ENDIF

Label   IF 1==1                         ; Illegal label
	   NOP
	ENDIF

AA = 1
BB = 1
CC = 1
DD = 1
EE = 1
FF = 1
GG = 1
HH = 1
II = 1

A1 = 1
B1 = 1
C1 = 1
D1 = 1
E1 = 1
F1 = 1
G1 = 1
H1 = 1
I1 = 1

	WHILE AA+BB+CC+DD+EE+FF+GG+HH + A1+B1+C1+D1+E1+F1+G1+H1 > 0
	   IF AA > 0
	      DATA                      "A+"
AA--
	   ELSE
	      DATA                      "A-"
	      IF BB > 0
		 DATA                   "B+"
BB--
	      ELSE
		 DATA                   "B-"
		 IF CC > 0
		    DATA                "C+"
CC--
		 ELSE
		    DATA                "C-"
		    IF DD > 0
		       DATA             "D+"
DD--
		    ELSE
		       DATA             "D-"
		       IF EE > 0
			  DATA          "E+"
EE--
		       ELSE
			  DATA          "E-"
			  IF FF > 0
			     DATA       "F+"
FF--
			  ELSE
			     DATA       "F-"
			     IF GG > 0
				DATA    "G+"
GG--
			     ELSE
				DATA    "G-"
				IF HH > 0
				   DATA "H+"
HH--
				ELSE
				   DATA "H-"
				   IF II > 0            ; Too deep - when level 8 is max
				      DATA      "I+"
II--
				   ELSE
				      DATA      "I-"
	   IF A1 > 0
	      DATA                      "A1+"
A1--
	   ELSE
	      DATA                      "A1-"
	      IF B1 > 0
		 DATA                   "B1+"
B1--
	      ELSE
		 DATA                   "B1-"
		 IF C1 > 0
		    DATA                "C1+"
C1--
		 ELSE
		    DATA                "C1-"
		    IF D1 > 0
		       DATA             "D1+"
D1--
		    ELSE
		       DATA             "D1-"
		       IF E1 > 0
			  DATA          "E1+"
E1--
		       ELSE
			  DATA          "E1-"
			  IF F1 > 0
			     DATA       "F1+"
F1--
			  ELSE
			     DATA       "F1-"
			     IF G1 > 0
				DATA    "G1+"
G1--
			     ELSE
				DATA    "G1-"
				IF H1 > 0		; Too deep
H1--
				ENDIF
			     ENDIF
			  ENDIF
		       ENDIF
		    ENDIF
		 ENDIF
	      ENDIF
	   ENDIF
				   ENDIF
				ENDIF
			     ENDIF
			  ENDIF
		       ENDIF
		    ENDIF
		 ENDIF
	      ENDIF
	   ENDIF
	ENDW

	IF 1==1         ; Missing ENDIF
	   NOP

	END
