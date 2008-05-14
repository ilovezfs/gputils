    list p=16f73
ifndef _TEST_INC
_TEST_INC = 1
    list
    extern foo
endif

    extern blah
    code
    movf blah, w
    end

