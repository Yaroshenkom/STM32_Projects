Stack_Size    	EQU     0x00000400 ;400h=1024


				AREA    STACK, NOINIT, READWRITE, ALIGN=3
Stack_Mem       SPACE   Stack_Size
__initial_sp


				AREA	RESET, DATA, READONLY
				EXPORT  __Vectors
                EXPORT  Reset_Handler			[WEAK]				
__Vectors
                DCD     __initial_sp            
                DCD     Reset_Handler            
__Vectors_End


				AREA	MYDATA, DATA, READWRITE
				ALIGN	4
Data_Mem       SPACE   0x00000400
	

				AREA	|.text| , CODE,	READONLY
				EXTERN	main
Reset_Handler 	PROC
				LDR	R0, = main
				BX	R0
ENDP
ALIGN			
END