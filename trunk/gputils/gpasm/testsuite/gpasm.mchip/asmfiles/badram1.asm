; BADRAM1.ASM

; This file tests legal usage of the BADRAM directive.

        LIST    P=16C622

TEST    MACRO
        MOVWF   06      ;OK
        MOVWF   07      ;WARN
        MOVWF   08      ;WARN
        MOVWF   09      ;WARN
        MOVWF   0A      ;OK
        MOVWF   0B      ;OK
        MOVWF   0C      ;OK
        MOVWF   0D      ;WARN
        MOVWF   0E      ;WARN

        MOVWF   1E      ;WARN
        MOVWF   1F      ;OK

        MOVWF   7F      ;OK

        MOVWF   80      ;MESS

        MOVWF   86      ;MESS
        MOVWF   87      ;WARN, MESS
        MOVWF   88      ;WARN, MESS
        MOVWF   89      ;WARN, MESS
        MOVWF   8A      ;MESS
        MOVWF   8B      ;MESS
        MOVWF   8C      ;MESS
        MOVWF   8D      ;WARN, MESS
        MOVWF   8E      ;MESS
        MOVWF   8F      ;WARN, MESS
        MOVWF   90      ;WARN, MESS

        MOVWF   9D      ;WARN, MESS
        MOVWF   9E      ;WARN, MESS
        MOVWF   9F      ;MESS
        MOVWF   0A0     ;MESS

        MOVWF   0BE     ;MESS
        MOVWF   0BF     ;MESS
        MOVWF   0C0     ;WARN, MESS
        ENDM

        TITLE   These should have only messages
        PAGE
        TEST

        TITLE   These should have warnings and messages
        PAGE
        __MAXRAM        0BF
        __BADRAM        07-09, 0D-1E
        __BADRAM        87-89, 8D, 8F-9E
        TEST

        TITLE   These should have only messages
        PAGE
        __MAXRAM        0BF
        TEST

        END
