	list	p=16c84
	movlw	0
	tris	6
	option
loop	sleep
	incf	6,1
	goto	loop
	end
