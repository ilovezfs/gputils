  processor 16f877

goto_macro macro
       pagesel page0
       goto page0
       pagesel page1
       goto page1
       pagesel page2
       goto page2
       pagesel page3
       goto page3
       pagesel $
       goto $
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
