; WHILE2.ASM

; This file tests maximums of the WHILE directive.

	LIST	P=17C44, R=DEC

;;;; Begin: Changed in gputils
;;;;	I = 256
I = 256
;;;; End: Changed in gputils
	WHILE I > 0	; Maximum iterations
	   DATA	I
;;;; Begin: Changed in gputils
;;;;	   I--
I--
;;;; End: Changed in gputils
	ENDW

;;;; Begin: Changed in gputils
;;;;	I = 10
I = 10
;;;; End: Changed in gputils
	WHILE I == 10	; Single pass, maximum lines internal
	   DATA	1
	   DATA	2
	   DATA	3
	   DATA	4
	   DATA	5
	   DATA	6
	   DATA	7
	   DATA	8
	   DATA	9
	   DATA	10
	   DATA	11
	   DATA	12
	   DATA	13
	   DATA	14
	   DATA	15
	   DATA	16
	   DATA	17
	   DATA	18
	   DATA	19
	   DATA	20
	   DATA	21
	   DATA	22
	   DATA	23
	   DATA	24
	   DATA	25
	   DATA	26
	   DATA	27
	   DATA	28
	   DATA	29
	   DATA	30
	   DATA	31
	   DATA	32
	   DATA	33
	   DATA	34
	   DATA	35
	   DATA	36
	   DATA	37
	   DATA	38
	   DATA	39
	   DATA	40
	   DATA	41
	   DATA	42
	   DATA	43
	   DATA	44
	   DATA	45
	   DATA	46
	   DATA	47
	   DATA	48
	   DATA	49
	   DATA	50
	   DATA	51
	   DATA	52
	   DATA	53
	   DATA	54
	   DATA	55
	   DATA	56
	   DATA	57
	   DATA	58
	   DATA	59
	   DATA	60
	   DATA	61
	   DATA	62
	   DATA	63
	   DATA	64
	   DATA	65
	   DATA	66
	   DATA	67
	   DATA	68
	   DATA	69
	   DATA	70
	   DATA	71
	   DATA	72
	   DATA	73
	   DATA	74
	   DATA	75
	   DATA	76
	   DATA	77
	   DATA	78
	   DATA	79
	   DATA	80
	   DATA	81
	   DATA	82
	   DATA	83
	   DATA	84
	   DATA	85
	   DATA	86
	   DATA	87
	   DATA	88
	   DATA	89
	   DATA	90
	   DATA	91
	   DATA	92
	   DATA	93
	   DATA	94
	   DATA	95
	   DATA	96
	   DATA	97
;;;; Begin: Added in gputils
I++
	ENDW
;;;; End: Added in gputils
;;;; Begin: Removed in gputils
;;;;	   DATA 98
;;;;	   I++
;;;;	ENDW
;;;; End: Removed in gputils
;	   DATA	99

;;;; Begin: Changed in gputils
;;;;	I = 1
I = 1
;;;; End: Changed in gputils
	WHILE I >= 0		; Maximum nesting
;;;; Begin: Changed in gputils
;;;;	   J = 1
J = 1
;;;; End: Changed in gputils
	   WHILE J >= 0
;;;; Begin: Changed in gputils
;;;;	      K = 1 
K = 1 
;;;; End: Changed in gputils
	      WHILE K >= 0
;;;; Begin: Changed in gputils
;;;;	         L = 1
L = 1
;;;; End: Changed in gputils
	         WHILE L >= 0
;;;; Begin: Changed in gputils
;;;;	            M = 1
M = 1
;;;; End: Changed in gputils
	            WHILE M >= 0
;;;; Begin: Changed in gputils
;;;;	               N = 1
N = 1
;;;; End: Changed in gputils
	               WHILE N >= 0
;;;; Begin: Changed in gputils
;;;;	                  O = 1
O = 1
;;;; End: Changed in gputils
	                  WHILE O >= 0
;;;; Begin: Changed in gputils
;;;;	                     P = 1
P = 1
;;;; End: Changed in gputils
	                     WHILE P >= 0
	                        DATA	((((((I*2 + J)*2 + K)*2 + L)*2 + M)*2 + N)*2 + O)*2 + P
;;;; Begin: Changed in gputils
;;;;	                        P--
P--
;;;; End: Changed in gputils
	                     ENDW
;;;; Begin: Changed in gputils
;;;;	                     O--
O--
;;;; End: Changed in gputils
	                  ENDW
;;;; Begin: Changed in gputils
;;;;	                  N--
N--
;;;; End: Changed in gputils
	               ENDW
;;;; Begin: Changed in gputils
;;;;	               M--
M--
;;;; End: Changed in gputils
	            ENDW
;;;; Begin: Changed in gputils
;;;;	            L--
L--
;;;; End: Changed in gputils
	         ENDW
;;;; Begin: Changed in gputils
;;;;	         K--
K--
;;;; End: Changed in gputils
	      ENDW
;;;; Begin: Changed in gputils
;;;;	      J--
J--
;;;; End: Changed in gputils
	   ENDW
;;;; Begin: Changed in gputils
;;;;	   I--
I--
;;;; End: Changed in gputils
	ENDW

	END
