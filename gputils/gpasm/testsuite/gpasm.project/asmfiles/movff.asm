; this file tests for invalid movff destinations

  processor 18f242

TOSU            EQU  H'0FFF'
TOSH            EQU  H'0FFE'
TOSL            EQU  H'0FFD'
PCL             EQU  H'0FF9'
  
  org 0
  
  movff 0x100, PCL   
  movff 0x100, TOSU  
  movff 0x100, TOSH   
  movff 0x100, TOSL   
  
  end
