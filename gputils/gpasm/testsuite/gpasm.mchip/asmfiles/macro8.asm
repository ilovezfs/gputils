;MACRO8.ASM

; This file tests illegal parameter substitution in macros.

        LIST    P=17C42

TEST    MACRO   NUM
        ADDLW   NUM
        ENDM

        TEST            ;Missing parameter
        TEST    1, 2    ;Too many parameters.

        END

