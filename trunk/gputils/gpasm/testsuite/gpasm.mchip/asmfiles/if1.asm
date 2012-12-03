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
;;;; Begin: Changed in gputils
;;;;	      AA--
AA--
;;;; End: Changed in gputils
	   ELSE
	      DATA                      "A-"
	      IF BB > 0
		 DATA                   "B+"
;;;; Begin: Changed in gputils
;;;;		 BB--
BB--
;;;; End: Changed in gputils
	      ELSE
		 DATA                   "B-"
		 IF CC > 0
		    DATA                "C+"
;;;; Begin: Changed in gputils
;;;;		    CC--
CC--
;;;; End: Changed in gputils
		 ELSE
		    DATA                "C-"
		    IF DD > 0
		       DATA             "D+"
;;;; Begin: Changed in gputils
;;;;		       DD--
DD--
;;;; End: Changed in gputils
		    ELSE
		       DATA             "D-"
		       IF EE > 0
			  DATA          "E+"
;;;; Begin: Changed in gputils
;;;;			  EE--
EE--
;;;; End: Changed in gputils
		       ELSE
			  DATA          "E-"
			  IF FF > 0
			     DATA       "F+"
;;;; Begin: Changed in gputils
;;;;			     FF--
FF--
;;;; End: Changed in gputils
			  ELSE
			     DATA       "F-"
			     IF GG > 0
				DATA    "G+"
;;;; Begin: Changed in gputils
;;;;				GG--
GG--
;;;; End: Changed in gputils
			     ELSE
				DATA    "G-"
				IF HH > 0
				   DATA "H+"
;;;; Begin: Changed in gputils
;;;;				   HH--
HH--
;;;; End: Changed in gputils
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
