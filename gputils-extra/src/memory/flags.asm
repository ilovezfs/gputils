;  Copyright (C) 2001  Carlos Nieves Onega <cnieves@confluencia.net>

;  This program is free software; you can redistribute it and/or modify
;  it under the terms of the GNU General Public License as published by
;  the Free Software Foundation; either version 2 of the License, or
;  (at your option) any later version.

;  This program is distributed in the hope that it will be useful,
;  but WITHOUT ANY WARRANTY; without even the implied warranty of
;  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;  GNU General Public License for more details.

;  You should have received a copy of the GNU General Public License
;  along with this program; if not, write to the Free Software
;  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
;
;**********************************************************************
;                                                                     *
;    File:          flags.asm                                         *
;    Version:       20011026                                          *
;                                                                     *
;    Author:   Carlos Nieves Onega                                    *
;                                                                     *
;                                                                     * 
;                                                                     *
;**********************************************************************
;                                                                     *
;       Required files:                                               *
;                                                                     *
;**********************************************************************
;                                                                     *
;    Notes:   This file contains routines to handle bit variables     *
;             All variables must be in a single block in data memory  *
;             Only an integer number of bytes will be used for the    *
;             bits table.                                             *
;                                                                     *
;**********************************************************************
; Changes:
;       20011026 (cnieves): Began writing.
;                           Functions tested (simulated):
;                               set_flag, clr_flag, set_all_flags, clr_all_flags, 
;                               look_flag, count_flags, bit_to_reg
;                           They work right.
;**********************************************************************

;#define        FLAGS_NUM       CADS_NUM-1      ; Begun at 0

;**********************************************************************
;       Function bit_to_reg
;       Description: Given a bit (the first is 0), returns the byte where is the
;               flag to look at in a register
;       Parameters:
;               WREG: flag number to look (the first is the 0).
;       Returns:
;               INDF, FSR: byte where the flag is.
;               RES_B2R: mask for the byte to look.
;       Use:
;               vars. TEMP_B2R, RES_B2R, 
;               regs FSR, INDF, W, STATUS
;**********************************************************************
                cblock 
TEMP_SAF:       0
TEMP_B2R:       1               ; Variable reused by set_all_flags and clr_all_flags
RES_B2R:        1
FLAGS_INI_ADDR: ((FLAGS_NUM+1) >> 3) + 1
                endc



bit_to_reg
                movwf   TEMP_B2R        
                movwf   RES_B2R         ; Stores the bit to look in 2 temp. vars.
                movlw   FLAGS_INI_ADDR  
                movwf   FSR             ; FSR= Flags block start address 
                bcf     STATUS,C
                rrf     TEMP_B2R,F
                bcf     STATUS,C
                rrf     TEMP_B2R,F
                bcf     STATUS,C
                rrf     TEMP_B2R,W      ; Gets the offset to the byte containing the flag.
                addwf   FSR,F           ; FSR=Address of the byte containing the flag
                movf    RES_B2R,W       ; 
                movwf   TEMP_B2R        ; temp. var. = Number of flag to look
                movlw   0x07
                andwf   TEMP_B2R,F      ; temp. var. between 0 and 7
                incf    TEMP_B2R,F      ; Bit a look (between 0 y 7) += 1 (for the next loop).
                                        ; Almacenar? el contador del n?mero de bit.
                bsf     STATUS,C        ; Because of rlf
                clrf    RES_B2R
BUC_B2R         rlf     RES_B2R,F       ; Result = 1 (initial cond. if the flag number is 0).
                bcf     STATUS,C        ; C = 0
                decfsz  TEMP_B2R,F      ; counter--
                goto BUC_B2R
                return


;**********************************************************************
;       Function set_flag
;       Description: sets a given flat to 1.
;       Parameters:
;               WREG: flag number to be modified.
;       Returns: nothing
;       Use:
;               func. bit_to_reg,
;               regs W, INDF, FSR
;**********************************************************************
set_flag
                call    bit_to_reg
                movf    RES_B2R,W       ; W = mask of the flag to be modified (bit = 1)
                iorwf   INDF,F          ; Sets the given bit.
                return


;**********************************************************************
;       Function clr_flag
;       Description: clears a given flag to 0.
;       Parameters:
;               WREG: flag number to be modified.
;       Returns: nothing
;       Use:
;               func. bit_to_reg,
;               regs W, INDF, FSR
;**********************************************************************
clr_flag
                call    bit_to_reg
                comf    RES_B2R,W       ; Complement to 1 the mask.
                                        ; W = mask of the bit to be modified (bit = 0)
                andwf   INDF,F          ; Clears the right bit.
                return

;**********************************************************************
;       Function look_flag
;       Description: returns the status of a given flag.
;       Parameters:
;               WREG: flag number to look.
;       Returns:
;               W = 0 , Z = 1, if the bit is 0.
;               W != 0, Z = 0, if the bit is 1.
;       Use:
;               func. bit_to_reg,
;               regs W, INDF, FSR
;**********************************************************************
look_flag
                call    bit_to_reg
                movf    RES_B2R,W       ; W = mask of the bit to look (bit = 1)
                andwf   INDF,W          ; In W is the bit to look, and the others to 0.
                return


;**********************************************************************
;       Function set_all_flags
;       Description: sets to 1 all flags.
;       Parameters: none.
;       Returns: nothing
;       Use:
;               regs W, INDF, FSR
;               var. temp TEMP_SAF
;**********************************************************************
set_all_flags
                movlw   FLAGS_NUM
                movwf   TEMP_SAF                ; TEMP_SAF = Number of bits to be modified
                bcf     STATUS,Z
                rrf     TEMP_SAF,F
                bcf     STATUS,C
                rrf     TEMP_SAF,F
                bcf     STATUS,C
                rrf     TEMP_SAF,F
                incf    TEMP_SAF,F              ; TEMP_SAF = Number of bits to be modified.
                movlw   FLAGS_INI_ADDR
                movwf   FSR                     ; FSR points to flag block
BUC_SAF         
                clrf    INDF
                comf    INDF,F                  ; Flags Byte = 0xff
                incf    FSR,F                   ; FSR points to the next address.
                decfsz  TEMP_SAF,F
                goto BUC_SAF
                return

;**********************************************************************
;       Function clr_all_flags
;       Description: clears to 0 all flags.
;       Parameters: none
;       Returns: nothing
;       Use:
;               regs W, INDF, FSR
;               var. temp TEMP_SAF
;**********************************************************************
clr_all_flags
                movlw   FLAGS_NUM
                movwf   TEMP_SAF                ; TEMP_SAF = Number of bits to be modified.
                bcf     STATUS,Z
                rrf     TEMP_SAF,F
                rrf     TEMP_SAF,F
                rrf     TEMP_SAF,F
                incf    TEMP_SAF,F              ; TEMP_SAF = Number of bits to be modified.
                movlw   FLAGS_INI_ADDR
                movwf   FSR                     ; FSR points to the flags block
BUC_CAF         clrf    INDF                    ; Flags byte= 0
                incf    FSR,F                   ; FSR points to the next address.
                decfsz  TEMP_SAF,F
                goto BUC_CAF
                return


;**********************************************************************
;       Function count_flags

;       Description: count the flags being 1
;       Parameters: none
;       Returns:
;               W: number of flags set.
;       Use:
;               regs W
;               var. temp. TEMP_CB, RES_CB
;               func. look_flag
;**********************************************************************
                cblock
TEMP_CB:        1
RES_CB:         1
                endc

count_flags
                clrf    RES_CB
                movlw   FLAGS_NUM
                movwf   TEMP_CB
BUC_CB          movf    TEMP_CB,W
                call    look_flag
                btfss   STATUS,Z
                incf    RES_CB,F
                decfsz  TEMP_CB,F
                goto BUC_CB
                movf    RES_CB,W
                return
