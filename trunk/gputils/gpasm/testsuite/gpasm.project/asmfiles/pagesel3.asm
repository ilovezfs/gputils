  processor 17c766

goto_macro macro
       pagesel page0
       lcall page0
       pagesel page1
       lcall page1
       pagesel page2
       lcall page2
       pagesel page3
       lcall page3
       endm
  
page0  org 0
       goto_macro

page1  org 801
       goto_macro

page2  org 1001
       goto_macro
  
page3  org 1801
       goto_macro
 
       end 
