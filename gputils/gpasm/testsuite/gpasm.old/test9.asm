	title "This should be the main Title"
	subtitle "A simple test for a simple Subtitle"

	    PROCESSOR	PIC16F877

	__CONFIG 0xfff8

counter macro
x_cnt = x_cnt + 1
        movlw x_cnt
        endm

        org 0
x_cnt   set 0
        counter
        counter
        counter
i = 0
	while (i<5)
	movlw	i
i = i + 1
	endw

	movwf  6

        end

				
	end
