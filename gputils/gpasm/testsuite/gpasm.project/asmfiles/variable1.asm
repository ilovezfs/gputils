     list p=16c84

value equ 0x20 
 
     constant number1=3+5, number2=0x20, number3=value+value, number4=value+5  
 
     movlw number1 
     movlw number2 
     movlw number3 
     movlw number4 

     variable test1=4
     movlw test1 
     variable test1=3
     movlw test1 
     variable test2,test3=6,test4=test1+test3
     movlw test2 
     movlw test3 
     movlw test3 
     
     end
