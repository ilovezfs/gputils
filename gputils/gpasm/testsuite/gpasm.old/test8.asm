	    PROCESSOR	PIC12C508

	__CONFIG 55
inc	macro	freg,distance
	if ((-2 <= distance) && (distance <= -1))
i = distance
			while	(i < 0)
				decf	freg,f
i = i + 1
			endw
	else 
		if	((0 <= distance) && (distance <= 2))
i = distance
			while	(i > 0)
				incf	freg,f
i = i - 1
			endw
		else
			movlw	distance
			addwf	freg,f
		endif
	endif
	endm
	
	org	0
	
	inc	18,-1
	inc	18,-2
	inc	18,-5
	inc	18,0
	inc	18,1
	inc	18,2
	inc	18,5
				
	end
