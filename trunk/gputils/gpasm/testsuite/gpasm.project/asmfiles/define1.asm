; this file tests the substitution of labels found in the defines table

  processor 16c84


  ifdef __16C84
    movlw 1
  endif

  ifndef __16C72
   movlw 1
  endif

pie equ 3

; make sure comments don't make their way into #defines
#define apple pie   ; this is a comment

    movlw apple

    fill apple,pie

#define oldlabel newlabel

oldlabel retlw 5

#undefine oldlabel

#define oldlabel nextlabel

oldlabel retlw 3

#define empty

  goto newlabel
  goto nextlabel

    end
