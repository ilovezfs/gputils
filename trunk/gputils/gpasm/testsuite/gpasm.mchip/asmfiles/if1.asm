; IF1.ASM 

; This file tests legal usage of the IF-ELSE-ENDIF directives.

	LIST    P=17C42

#define Def1    5
Value1  EQU     10

	IF 1 == 1
	   DATA         "OK"
	ELSE
	   ERROR        "Bad condition evaluation"
	ENDIF

	IF 1 == 2 
	   ERROR        "Bad condition evaluation"
	ELSE
	   DATA         "OK"
	ENDIF

	IF 1 < 2 
	   DATA         "OK"
	ELSE
	   ERROR        "Bad condition evaluation"
	ENDIF

	IF 1 > 2 
	   ERROR        "Bad condition evaluation"
	ELSE
	   DATA         "OK"
	ENDIF

	IF Def1 == 5
	   DATA         "OK"
	ELSE
	   ERROR        "Bad condition evaluation"
	ENDIF

	IF Value1 == 10
	   DATA         "OK"
	ELSE
	   ERROR        "Bad condition evaluation"
	ENDIF

	IF 1==1
	   IF 2==2
	      DATA      "OK"
	   ELSE
	      ERROR     "Bad condition evaluation"
	   ENDIF
	ELSE
	   IF 2==2
	      ERROR     "Bad condition evaluation"
	   ELSE
	      ERROR     "Bad condition evaluation"
	   ENDIF
	ENDIF

	IF 1==1
	   IF 2!=2
	      ERROR     "Bad condition evaluation"
	   ELSE
	      DATA      "OK"
	   ENDIF
	ELSE
	   IF 2==2
	      ERROR     "Bad condition evaluation"
	   ELSE
	      ERROR     "Bad condition evaluation"
	   ENDIF
	ENDIF

	IF 1!=1
	   IF 2==2
	      ERROR     "Bad condition evaluation"
	   ELSE
	      ERROR     "Bad condition evaluation"
	   ENDIF
	ELSE
	   IF 2==2
	      DATA      "OK"
	   ELSE
	      ERROR     "Bad condition evaluation"
	   ENDIF
	ENDIF

	IF 1!=1
	   IF 2==2
	      ERROR     "Bad condition evaluation"
	   ELSE
	      ERROR     "Bad condition evaluation"
	   ENDIF
	ELSE
	   IF 2!=2
	      ERROR     "Bad condition evaluation"
	   ELSE
	      DATA      "OK"
	   ENDIF
	ENDIF


AA = 1
BB = 1
CC = 1
DD = 1
EE = 1
FF = 1
GG = 1
HH = 1

	WHILE AA+BB+CC+DD+EE+FF+GG+HH > 0
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
				ENDIF
			     ENDIF
			  ENDIF
		       ENDIF
		    ENDIF
		 ENDIF
	      ENDIF
	   ENDIF
	ENDW

	END
