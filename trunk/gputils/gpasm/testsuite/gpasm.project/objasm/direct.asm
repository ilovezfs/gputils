    list p=16f877

.assert macro x
    .direct "a", x
    endm

    code
    .assert "(portb & 1) == 1, \"*** FAILED digital stimulus test-interrupt not on rising edge... and prolonging the string to be really, reallly, realllly, reallllly, realllllly, reallllllly, realllllllly,  reallllllllly, realllllllllly looooooooooooooooooooooong!\""
    retlw   1
    .direct 'A',"The direct command"

    end
