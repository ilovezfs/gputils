     list p=16c84

; Make sure directives are no longer case sensitive

     Title "test file"
     SubTitle "for various changes"  
     #DeFiNe frogger 2

; Defines can now start anywhere

#define num1 1
 #define num2 2
	#define num3 3
                 #define num4 4

anum  equ num1
bnum  equ num2
cnum  equ num3
dnum  equ num4

     movlw anum
     movlw bnum
     movlw cnum
     movlw dnum

; the undefine directive is now supported

#undefine num1

#define num1 5

     movlw num1
     
; Another example of how directives are not case sensitive
     
     eNd
