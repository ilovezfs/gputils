
;*********************************************************************
;                                                                    *
;                       Software License Agreement                   *
;                                                                    *
;   The software supplied herewith by Microchip Technology           *
;   Incorporated (the "Company") for its PICmicro® Microcontroller   *
;   is intended and supplied to you, the Company’s customer, for use *
;   solely and exclusively on Microchip PICmicro Microcontroller     *
;   products. The software is owned by the Company and/or its        *
;   supplier, and is protected under applicable copyright laws. All  *
;   rights are reserved. Any use in violation of the foregoing       *
;   restrictions may subject the user to criminal sanctions under    *
;   applicable laws, as well as to civil liability for the breach of *
;   the terms and conditions of this license.                        *
;                                                                    *
;   THIS SOFTWARE IS PROVIDED IN AN "AS IS" CONDITION.  NO           *
;   WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING,    *
;   BUT NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND    *
;   FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE     *
;   COMPANY SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL,  *
;   INCIDENTAL OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.  *
;                                                                    *
;*********************************************************************

;*********************************************************************
;                                                                    *
;   Filename:       i2ccomm1.inc                                     *
;   Date:           07/18/2000                                       *
;   Revision:       1.00                                             *
;                                                                    *
;   Tools:          MPLAB   5.11.00                                  *
;                   MPLINK  2.10.00                                  *
;                   MPASM   2.50.00                                  *
;                                                                    *
;*********************************************************************
;                                                                    *
;   Notes:                                                           *
;	                                                                 *
;	This file is to be included in the <main.asm> file. The          *
;   <main.asm> notation represents the file which has the            *
;   subroutine calls for the functions 'service_i2c' and 'init_i2c'. *   
;                                                                    * 
;                                                                    *
;********************************************************************/

		#include  "flags.inc"            ; required include file






		GLOBAL		write_string         ; make variable viewable for other modules
		GLOBAL 		read_string          ; make variable viewable for other modules

		EXTERN		sflag_event          ; reference linkage for variable  
		EXTERN		eflag_event          ; reference linkage for variable
		EXTERN		i2cState             ; reference linkage for variable
		EXTERN		read_count           ; reference linkage for variable
		EXTERN		write_count          ; reference linkage for variable
		EXTERN		write_ptr            ; reference linkage for variable
		EXTERN		read_ptr             ; reference linkage for variable
		EXTERN		temp_address         ; reference linkage for variable

		EXTERN		init_i2c             ; reference linkage for function
		EXTERN		service_i2c          ; reference linkage for function

  


;*********************************************************************
;                                                                    *
;   Additional notes on variable usage:                              *
;                                                                    *
;   The variables listed below are used within the function          *
;   service_i2c. These variables must be initialized with the        *
;   appropriate data from within the calling file. In this           *
;   application code the main file is 'mastri2c.asm'. This file      *
;   contains the function calls to service_i2c. It also contains     *
;   the function for initializing these variables, called 'init_vars'*
;                                                                    *
;   To use the service_i2c function to read from and write to an     *
;   I2C slave device, information is passed to this function via     *
;   the following variables.                                         *
;                                                                    *
;                                                                    *
;   The following variables are used as function parameters:         *
;                                                                    *
;   read_count   - Initialize this variable for the number of bytes  *
;                  to read from the slave I2C device.                *      
;   write_count  - Initialize this variable for the number of bytes  *
;                  to write to the slave I2C device.                 *   
;   write_ptr    - Initialize this variable with the address of the  *
;                  data string or data byte to write to the slave    *
;                  I2C device.                                       *     
;   read_ptr     - Initialize this variable with the address of the  *
;                  location for storing data read from the slave I2C *
;                  device.                                           *
;   temp_address - Initialize this variable with the address of the  *
;                  slave I2C device to communicate with.             *
;                                                                    *
;                                                                    *
;   The following variables are used as status or error events       *
;                                                                    *
;   sflag_event  - This variable is implemented for status or        *
;                  event flags. The flags are defined in the file    *
;                  'flags.inc'.                                      *   
;   eflag_event  - This variable is implemented for error flags. The *
;                  flags are defined in the file 'flags.inc'.        * 
;                                                                    *
;                                                                    *
;   The following variable is used in the state machine jumnp table. *
;                                                                    *
;   i2cState     - This variable holds the next I2C state to execute.*
;                                                                    *
;*********************************************************************
