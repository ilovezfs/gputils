; This file is used to test relocatable objects with the enhanced 14 bit
; instruction set. Some of the expressions are possible, but not very useful.

    list p=12f1822,r=dec
    extern file, address

FSR0 equ H'0004'
FSR1 equ H'0006'

    code

    addfsr FSR0,-32
    addfsr FSR0,31
    addfsr FSR1,-32
    addfsr FSR1,31
    addlw file
    addwf file,1
    addwfc file,1
    andlw file
    andwf file,1
    asrf file,1
    banksel file
    bcf file,7
    bra address
    brw
    bsf file,7
    btfsc file,1
    btfss file,1
    call address
    callw
    clrf file
    clrw
    clrwdt
    comf file,1
    decf file,1
    decfsz file,1
    pagesel address
    goto address
    incf file,1
    incfsz file,1
    iorlw file
    iorwf file,1
    lslf file,1
    lsrf file,1

    moviw ++FSR0
    moviw --FSR0
    moviw FSR0++
    moviw FSR0--

    movf  file,1
    movlb file
    movlp file
    movlw file
    movwf file

    movwi ++FSR0
    movwi --FSR0
    movwi FSR0++
    movwi FSR0--

    nop
    option
    reset
    retfie
    retlw file
    return
    rlf file,1
    rrf file,1
    sleep
    sublw file
    subwf file,1
    subwfb file,1
    swapf file,1
    tris file
    xorlw file
    xorwf file,1

    end
