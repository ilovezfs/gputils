        processor 16f1933
        radix   dec

FSR0    equ     H'0004'
FSR1    equ     H'0006'

; INSN_CLASS_MOVINDF
        moviw   ++FSR0
        moviw   --FSR0
        moviw   FSR0++
        moviw   FSR0--

        moviw   ++FSR1
        moviw   --FSR1
        moviw   FSR1++
        moviw   FSR1--

        movwi   ++FSR0
        movwi   --FSR0
        movwi   FSR0++
        movwi   FSR0--

        movwi   ++FSR1
        movwi   --FSR1
        movwi   FSR1++
        movwi   FSR1--

moviwx  macro   val
        moviw   val[FSR0]
        moviw   val[FSR1]
        endm

        moviwx  0
        moviwx  1
        moviwx  2
        moviwx  3
        moviwx  4
        moviwx  5
        moviwx  6
        moviwx  7
        moviwx  8
        moviwx  9
        moviwx  10
        moviwx  11
        moviwx  12
        moviwx  13
        moviwx  14
        moviwx  15
        moviwx  16
        moviwx  17
        moviwx  18
        moviwx  19
        moviwx  20
        moviwx  21
        moviwx  22
        moviwx  23
        moviwx  24
        moviwx  25
        moviwx  26
        moviwx  27
        moviwx  28
        moviwx  29
        moviwx  30
        moviwx  31

        moviwx  -1
        moviwx  -2
        moviwx  -3
        moviwx  -4
        moviwx  -5
        moviwx  -6
        moviwx  -7
        moviwx  -8
        moviwx  -9
        moviwx  -10
        moviwx  -11
        moviwx  -12
        moviwx  -13
        moviwx  -14
        moviwx  -15
        moviwx  -16
        moviwx  -17
        moviwx  -18
        moviwx  -19
        moviwx  -20
        moviwx  -21
        moviwx  -22
        moviwx  -23
        moviwx  -24
        moviwx  -25
        moviwx  -26
        moviwx  -27
        moviwx  -28
        moviwx  -29
        moviwx  -30
        moviwx  -31
        moviwx  -32

movwix  macro   val
        movwi   val[FSR0]
        movwi   val[FSR1]
        endm

        movwix  0
        movwix  1
        movwix  2
        movwix  3
        movwix  3
        movwix  4
        movwix  5
        movwix  6
        movwix  7
        movwix  8
        movwix  9
        movwix  10
        movwix  11
        movwix  12
        movwix  13
        movwix  14
        movwix  15
        movwix  16
        movwix  17
        movwix  18
        movwix  19
        movwix  20
        movwix  21
        movwix  22
        movwix  23
        movwix  24
        movwix  25
        movwix  26
        movwix  27
        movwix  28
        movwix  29
        movwix  30
        movwix  31

        movwix  -1
        movwix  -2
        movwix  -3
        movwix  -3
        movwix  -4
        movwix  -5
        movwix  -6
        movwix  -7
        movwix  -8
        movwix  -9
        movwix  -10
        movwix  -11
        movwix  -12
        movwix  -13
        movwix  -14
        movwix  -15
        movwix  -16
        movwix  -17
        movwix  -18
        movwix  -19
        movwix  -20
        movwix  -21
        movwix  -22
        movwix  -23
        movwix  -24
        movwix  -25
        movwix  -26
        movwix  -27
        movwix  -28
        movwix  -29
        movwix  -30
        movwix  -31
        movwix  -32

; INSN_CLASS_LITFSR
addfsrx macro   val
        addfsr  FSR0,val
        addfsr  FSR1,val
        endm

        addfsrx 0
        addfsrx 1
        addfsrx 2
        addfsrx 3
        addfsrx 4
        addfsrx 5
        addfsrx 6
        addfsrx 7
        addfsrx 8
        addfsrx 9
        addfsrx 10
        addfsrx 11
        addfsrx 12
        addfsrx 13
        addfsrx 14
        addfsrx 15
        addfsrx 16
        addfsrx 17
        addfsrx 18
        addfsrx 19
        addfsrx 20
        addfsrx 21
        addfsrx 22
        addfsrx 23
        addfsrx 24
        addfsrx 25
        addfsrx 26
        addfsrx 27
        addfsrx 28
        addfsrx 29
        addfsrx 30
        addfsrx 31

        addfsrx -1
        addfsrx -2
        addfsrx -3
        addfsrx -4
        addfsrx -5
        addfsrx -6
        addfsrx -7
        addfsrx -8
        addfsrx -9
        addfsrx -10
        addfsrx -11
        addfsrx -12
        addfsrx -13
        addfsrx -14
        addfsrx -15
        addfsrx -16
        addfsrx -17
        addfsrx -18
        addfsrx -19
        addfsrx -20
        addfsrx -21
        addfsrx -22
        addfsrx -23
        addfsrx -24
        addfsrx -25
        addfsrx -26
        addfsrx -27
        addfsrx -28
        addfsrx -29
        addfsrx -30
        addfsrx -31
        addfsrx -32

; INSN_CLASS_OPWF7

MAXROM = H'07FF'
MAXRAM = H'007F'

        addwfc  0
        addwfc  0,1
        addwfc  0,F
        addwfc  0,0
        addwfc  0,W
        addwfc  MAXRAM

        asrf    0
        asrf    0,1
        asrf    0,F
        asrf    0,0
        asrf    0,W
        asrf    MAXRAM

        lslf    0
        lslf    0,1
        lslf    0,F
        lslf    0,0
        lslf    0,W
        lslf    MAXRAM

        lsrf    0
        lsrf    0,1
        lsrf    0,F
        lsrf    0,0
        lsrf    0,W
        lsrf    MAXRAM

        subwfb  0
        subwfb  0,1
        subwfb  0,F
        subwfb  0,0
        subwfb  0,W
        subwfb  MAXRAM

; INSN_CLASS_RBRA9

        bra     $+256
        bra     $-255

; INSN_CLASS_IMPLICIT
        brw
        callw
        reset

; INSN_CLASS_LIT5
        movlb   0
        movlb   31

; INSN_CLASS_LIT7
        movlp   0
        movlp   127

; HIGH
        cblock  0x10
        table_data: 1
        endc

        data    HIGH table_code
        data    HIGH table_data

table_code
        end
