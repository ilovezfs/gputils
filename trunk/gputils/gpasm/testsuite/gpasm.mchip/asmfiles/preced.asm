; PRECED.ASM

; This file tests operator precedence.  Note that assignment operators
; cannot be embedded.

	LIST    P=17C44,R=HEX

	DATA    HIGH  102 + 101         ; 102
	DATA    HIGH (102 + 101)        ; 2
	DATA    LOW   102 + 101         ; 103
	DATA    LOW  (102 + 101)        ; 3

	DATA    1 + 2 * 3       ; 7
	DATA    1 + (2*3)       ; 7
	DATA    (1+2) * 3       ; 9

	DATA    8 - 3 * 2       ; 2
	DATA    8 - (3*2)       ; 2
	DATA    (8-3) * 2       ; 0A

	DATA    4 + 2 / 2       ; 5
	DATA    4 + (2/2)       ; 5
	DATA    (4+2) / 2       ; 3

	DATA    9 - 6 / 3       ; 7
	DATA    9 - (6/3)       ; 7
	DATA    (9-6) / 3       ; 1

	DATA    5 + 9 % 2       ; 6
	DATA    5 + (9%2)       ; 6
	DATA    (5+9) % 2       ; 0

	DATA    9 - 5 % 3       ; 7
	DATA    9 - (5%3)       ; 7
	DATA    (9-5) % 3       ; 1

	DATA    1 + 1 << 3      ; 10
	DATA    (1+1) << 3      ; 10
	DATA    1 + (1<<3)      ; 9

	DATA    2 + 6 & 2       ; 0
	DATA    (2+6) & 2       ; 0
	DATA    2 + (6&2)       ; 4

	DATA    1 + 3 | 4       ; 4
	DATA    (1+3) | 4       ; 4
	DATA    1 + (3|4)       ; 8

	DATA    1 + 7 ^ 8       ; 0
	DATA    (1+7) ^ 8       ; 0
	DATA    1 + (7^8)       ; 10

	DATA    0x0E | 1 ^ 2    ; 0D
	DATA    (0x0E|1) ^ 2    ; 0D
	DATA    0x0E | (1^2)    ; 0F

	DATA    1 + 3 == 2 + 2  ; 1
	DATA    (1+3) == (2+2)  ; 1
	DATA    1 + (3==2) + 2  ; 3

	DATA    6 & 2 == 6 & 3  ; 0
	DATA    6 & (2==6) & 3  ; 0
	DATA    (6&2) == (6&3)  ; 1

	DATA    2 == 2 && 0 + 1         ; 1
	DATA    (2==2) && (0+1)         ; 1
	DATA    2 == (2 && 0 + 1)       ; 0

	DATA    1 || 0 && 0     ; 1
	DATA    1 || (0&&0)     ; 1
	DATA    (1||0) && 0     ; 0

	END

