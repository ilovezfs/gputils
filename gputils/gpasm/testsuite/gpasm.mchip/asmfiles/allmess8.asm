;ALLMESS8.ASM
;	This file tests for messages generated from command line or
;	interface options.  It must be assembled with the command line 
;	options /W1 /M.

	list	p=16c54

	errorlevel	2		; Message 308 Warning level superceded by command line value.

	expand				; Message 309 Macro expansion superceded by commandline.

	end	
