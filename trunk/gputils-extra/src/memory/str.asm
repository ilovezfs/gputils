;  Copyright (C) 2001  Carlos Nieves Onega 

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

;**********************************************************************
;                                                                     
;    File:          str.asm                                           
;    Version:       20011030
;                                                                     
;    Author:   Carlos Nieves Onega                                     
;                                                                     
;                                                                      
;                                                                     
;**********************************************************************
;                                                                     
;       Required files:                                          
;               Flags handle routines (flags.asm, 20011026)
;                                                                     
;**********************************************************************
;                                                                     
;    Notes:   This file contains routines to handle strings.
;             Strings must be in a single block in program memory.
;                                                                     
;**********************************************************************
; Changes:
;       20011030 (cnieves): modified get_id_str_serie so it doesn't care about 
;                       CR y LF characters.
;       20011029 (cnieves): began writing.
;                       Functions tested:
;                               getchar_strrom and get_id_str_serie.
;                       Simulated. They work fine.
;**********************************************************************

;**********************************************************************
; Strings to be compared must be stored in a single block in memory.
; For each string, it must be stored, sorted as shown, the following:
;       * String length (1 byte): number 'n' of characters of the string.
;       * String Id. (1 byte): there mustn't be two strings with the same id.
;               Id must be greater than 0. The ids. begin with number 1 and 
;               must be correlative.
;       * Characters of the string: ( 'n' bytes).
;**********************************************************************
CADS_ADDR
                TABLE_JUMP
ID_CAD  set     1
                                ;   Length     Identifier     String
CAD1    equ     ID_CAD
                                DT      d'4',   ID_CAD,         "CAD1"
ID_CAD  set     ID_CAD + 1
CAD2    equ     ID_CAD
                                DT      d'12',  ID_CAD,         "This is cad2"
; The last string mustn't increase ID_CAD

; Define CADS_NUM, which will be necessary to reserve space for flags.
#define         CADS_NUM        ID_CAD


;**********************************************************************
;       Function getchar_strrom
;       Description: given the id. of the string, returns the character 
;               at the position given in WREG (begun with 0).
;       Parameters:
;               WREG: position of the character within the string (the first one is 0).
;               ID_CAD_GC: string id.. Always must be greater than 0.
;       Returns:
;               WREG: character at the given position. If the id. was not found
;               or if the length is less than the given position, returns 0xFF
;               Z = 1   : operation not valid (string or position invalid).
;               Z = 0   : operation completed succesfully.
;       Use:
;               var. temp. num_cad, id_cad, pos_char.
;               regs. W, INDF, FSR
;**********************************************************************
        cblock
CAD_NUM_GC:     1
ID_CAD_GC:      1
POS_CHAR_GC:    1
TABLE_POS_GC:   1
        endc

getchar_strrom
                movwf   POS_CHAR_GC
                clrf    TABLE_POS_GC
                movlw   CADS_NUM
                addlw   1               ; Adds 1 because of the loop particularities
                movwf   CAD_NUM_GC      ; num_cad = number of strings stored
BUC_GC          
                decfsz  CAD_NUM_GC,F
                goto    LABEL1_GC
                goto    NO_ENC_GC       ; If all strings were looked, it was not found.
LABEL1_GC               
                movlw   d'1'
                addwf   TABLE_POS_GC,W  ; W points to the string id.
                call    CADS_ADDR       
                                        ; W = string id.
                xorwf   ID_CAD_GC,W     ; compare the string id. with the parameter
                bz      ENC_GC          ; If it's 0, the string was found
                
                movf    TABLE_POS_GC,W  ; W = pointer to the length
                call    CADS_ADDR
                                        ; W = length of the string
                addwf   TABLE_POS_GC,F  ; TABLE_POS_GC += string length
                movlw   d'2'
                addwf   TABLE_POS_GC,F  ; TABLE_POS_GC += 3 (length, identifier)
                                        ; with this, TABLE_POS_GC points to the "struct" of the next string
                goto    BUC_GC
                

ENC_GC          
                movf    TABLE_POS_GC,W
                call    CADS_ADDR
                                        ; W = length
                subwf   POS_CHAR_GC,W   ; W = position (began with 0) - length (began with 1)
                bc      NO_ENC_GC

                movlw   d'2'    
                addwf   TABLE_POS_GC,F  ; FSR points to the beginning of the string

                movf    POS_CHAR_GC,W
                addwf   TABLE_POS_GC,W  ; FSR points to the wanted character
                call    CADS_ADDR       
                                        ; W = character
                bcf     STATUS,Z
                return

NO_ENC_GC       
                movlw   0xff
                bsf     STATUS,Z
                return



;**********************************************************************
;       Function get_id_str_serie
;       Description: Returns in WREG the id. of the string received
;               from the serial port (or 0 if it doesn't match).
;       Parameters: none
;       Return:
;               WREG: identifier of the received string. (0 if it's none of the stored).
;       Use:
;               Flags handle routines: set_all_flags, count_flags, look_flag
;**********************************************************************
        cblock
NUM_CHAR_GISS:  1
NUM_CAD_GISS:   1
CHAR_SERIE:     1
TEMP_GISS:      1
        endc

REC_SALE_GISS   decfsz  TEMP_GISS,1
                goto    REC_BUC_GISS
                retlw   0
get_id_str_serie
                call    set_all_flags   ; By now, all strings are valid. FIXME: only set those related with strings
                clrf    NUM_CHAR_GISS   ; Initialice the character to look to 0.
BUC_OTRO_CHAR_GISS
                movlw   0x80            ;Times to wait for a serial character
                movwf   TEMP_GISS       
REC_BUC_GISS    
                call    RX_SERIE        ; getchar(serie) You should change this to your needs.
                iorlw   0               
                bz      REC_SALE_GISS
                movf    DATA_IN,w

                movwf   CHAR_SERIE
                movlw   CR
                xorwf   CHAR_SERIE,W
                bz      BUC_OTRO_CHAR_GISS      ; If it's CR, ignore it
                movlw   LF
                xorwf   CHAR_SERIE,W
                bz      BUC_OTRO_CHAR_GISS      ; If it's LF, ignore it

                movlw   d'1'
                movwf   NUM_CAD_GISS    ; String Id. to analyze
BUC_CAD_GISS
                movlw   d'1'
                subwf   NUM_CAD_GISS,W
                call    look_flag
                bz      OTRA_CAD_GISS
                
                movf    NUM_CAD_GISS,W
                movwf   ID_CAD_GC       ; ID_CAD_GC = Id. to look
                movf    NUM_CHAR_GISS,W ; W = number of character to test
                call    getchar_strrom
                addlw   d'0'
                bz      ENC_GISS        ; If this character is 0 (string end), it was founded
                xorwf   CHAR_SERIE,W    ; compare it with the received character.
                bnz     NO_CAD_GISS     ; If they don't match, this string is not valid.

                movf    NUM_CAD_GISS,W
                movwf   ID_CAD_GC       ; ID_CAD_GC = Id. to look
                movlw   d'1'
                addwf   NUM_CHAR_GISS,W ; W = number of the character to look + 1
                call    getchar_strrom  ; Look the next character.
                xorlw   0xff
                bz      ENC_GISS        ; If it's 0xff (string end), it was founded
                goto    OTRA_CAD_GISS

NO_CAD_GISS     movlw   d'1'
                subwf   NUM_CAD_GISS,W
                call    clr_flag
                goto    OTRA_CAD_GISS

OTRA_CAD_GISS
                call    count_flags     ; Look how many strings were not compared yet.
                addlw   d'0'
                bz      NO_ENC_GISS     ; If none, then it was not founded.

                movlw   d'1'
                addwf   NUM_CAD_GISS,F
                movlw   CADS_NUM
                subwf   NUM_CAD_GISS,W  ; Compare the string id. de cadena to look with the number of strings
                bnc     BUC_CAD_GISS    ; Si it's negative (C=0), continues testing the next string
                bz      BUC_CAD_GISS    ; idem if it's zero

                movlw   d'1'
                addwf   NUM_CHAR_GISS,F ; NUM_CHAR_GISS++ (looks the next character)
                goto    BUC_OTRO_CHAR_GISS

NO_ENC_GISS
                retlw   0
ENC_GISS
                movf    NUM_CAD_GISS,W
                return
