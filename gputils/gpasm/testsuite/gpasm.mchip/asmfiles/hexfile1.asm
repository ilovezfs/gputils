;SSR 3351
;
; Generate hex file with maximum fragmentation (16C74A sized)

	LIST    P=16C74A

n SET 0
	WHILE n < 0x10
m SET 0
	WHILE m < 0x100
	org     n * 0x100 + m
	data	$
m SET m + 2
	ENDW
n SET n + 1
	ENDW
	END  


