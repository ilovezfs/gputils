; OPERAT1.ASM

; This file tests all operators for legal usage.

	LIST	P=16C54,R=HEX

	DATA	(1)			; 1
	DATA	!1			; 0
	DATA	!0			; 1
	DATA	-2			; -2
	DATA	~7			; FFF8
	DATA	HIGH 1234		; 12
	DATA	LOW 1234		; 34
	DATA	2*3			; 6
	DATA	3 * 4			; 0C
	DATA	9/3			; 3
	DATA	8 / 2			; 4
	DATA	9%4			; 1
	DATA	7 % 5			; 2
	DATA	1+2			; 3
	DATA	3 + 4			; 7
	DATA	8-3			; 5
	DATA	7 - 1			; 6
	DATA	1<<2			; 4
	DATA	2 << 2			; 8
	DATA	8>>2			; 2
	DATA	8 >> 1			; 4
	DATA	2>=1			; 1
	DATA	3 >= 5			; 0
	DATA	5>4			; 1
	DATA	5 > 6			; 0
	DATA	7<9			; 1
	DATA	4 < 3			; 0
	DATA	8<=8			; 1
	DATA	9 <= 3			; 0
	DATA	3==3			; 1
	DATA	7 == 5			; 0
	DATA	7!=9			; 1
	DATA	8 != 8			; 0
	DATA	7&4			; 4
	DATA	8 & 2			; 0
	DATA	8^2			; 0a
	DATA	7 ^ 1			; 6
	DATA	2|1			; 3
	DATA	4 | 1			; 5
	DATA	1&&1			; 1
	DATA	1 && 0			; 0
	DATA	1||0			; 1
	DATA	0 || 0			; 0

;;;; Begin: Changed in gputils
;;;;	I = 4
I = 4
;;;; End: Changed in gputils
	DATA	I			; 4
;;;; Begin: Changed in gputils
;;;;	I += 1
I += 1
;;;; End: Changed in gputils
	DATA	I			; 5
;;;; Begin: Changed in gputils
;;;;	I -= 2
I -= 2
;;;; End: Changed in gputils
	DATA	I			; 3
;;;; Begin: Changed in gputils
;;;;	I *= 2
I *= 2
;;;; End: Changed in gputils
	DATA	I			; 6
;;;; Begin: Changed in gputils
;;;;	I /= 3
I /= 3
;;;; End: Changed in gputils
	DATA	I			; 2
;;;; Begin: Changed in gputils
;;;;	I++
I++
;;;; End: Changed in gputils
	DATA	I			; 3
;;;; Begin: Changed in gputils
;;;;	I %= 2
I %= 2
;;;; End: Changed in gputils
	DATA	I			; 1
;;;; Begin: Changed in gputils
;;;;	I <<= 3
I <<= 3
;;;; End: Changed in gputils
	DATA	I			; 8
;;;; Begin: Changed in gputils
;;;;	I >>= 1
I >>= 1
;;;; End: Changed in gputils
	DATA	I			; 4
;;;; Begin: Changed in gputils
;;;;	I--
I--
;;;; End: Changed in gputils
	DATA	I			; 3
;;;; Begin: Changed in gputils
;;;;	I &= 2
I &= 2
;;;; End: Changed in gputils
	DATA	I			; 2
;;;; Begin: Changed in gputils
;;;;	I |= 4
I |= 4
;;;; End: Changed in gputils
	DATA	I			; 6
;;;; Begin: Changed in gputils
;;;;	I ^= 7
I ^= 7
;;;; End: Changed in gputils
	DATA	I			; 1

	DATA	$

	END
	

