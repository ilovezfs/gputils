; OPERAT2.ASM

; This file tests all operators for illegal usage.

	LIST	P=16C54,R=HEX

	DATA	(1			; Missing paren
	DATA	1)			; Missing paren
	DATA	!			; No value
	DATA	-			; No value
	DATA	~			; No value
	DATA	HIGH 			; No value
	DATA	LOW 			; No value
	DATA	2*			; Missing value
	DATA	 * 4			; Missing value
	DATA	9/			; Missing value
	DATA	 / 2			; Missing value
	DATA	9%			; Missing value
	DATA	 % 5			; Missing value
	DATA	1+			; Missing value
	DATA	 + 4			; Missing value
	DATA	8-			; Missing value
	DATA	 - 1			; Missing value
	DATA	1<<			; Missing value
	DATA	 << 2			; Missing value
	DATA	8>>			; Missing value
	DATA	 >> 1			; Missing value
	DATA	2>=			; Missing value
	DATA	 >= 5			; Missing value
	DATA	5>			; Missing value
	DATA	 > 6			; Missing value
	DATA	7<			; Missing value
	DATA	 < 7			; Missing value
	DATA	8<=			; Missing value
	DATA	 <= 3			; Missing value
	DATA	3==			; Missing value
	DATA	 == 5			; Missing value
	DATA	7!=			; Missing value
	DATA	 != 8			; Missing value
	DATA	7&			; Missing value
	DATA	 & 2			; Missing value
	DATA	8^			; Missing value
	DATA	 ^ 1			; Missing value
	DATA	2|			; Missing value
	DATA	 | 1			; Missing value
	DATA	1&&			; Missing value
	DATA	 && 0			; Missing value
	DATA	1||			; Missing value
	DATA	 || 0			; Missing value

I = 
 = 4
I += 
 += 1
I -= 
 -= 2
I *= 
 *= 2
I /= 
 /= 3
I %= 
 %= 2
I <<= 
 <<= 3
I >>= 
 >>= 1
I &= 
 &= 2
I |= 
 |= 4
I ^= 
 ^= 7

I = J				; Undefined symbol
 $

	END
	

