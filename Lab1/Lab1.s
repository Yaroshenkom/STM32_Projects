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
QA				DCD		0
QL				DCD		0

                AREA    MYCODE , CODE, READONLY	
;Define X, Y and Z for arithmetical calculations
X				EQU		0x053
Y				EQU		0x034
Z				EQU		0x05
;Define A, B and C for logical calculations
A__				DCD		0x055
B__				DCD		0x030
C__				DCD		0x021
D__				DCD		0x0E0
Reset_Handler	PROC
;Put X, Y and Z into registers.
				MOV 	R1,	#X
				MOV 	R2,	#Y
				MOV 	R3,	#Z
				
;Put A, B, C and D into registers
				LDR		R0, =A__
				LDR		R6, [R0]
				LDR		R0, =B__
				LDR		R7, [R0]
				LDR		R0, =C__
				LDR		R8, [R0]
				LDR		R0, =D__
				LDR		R9, [R0]
;Arithmetical calculations
				ADD		R4,	R1,	R3
				SUB		R4,	R4,	R2
				ADD		R5,	R4,	R1
				SUB		R5,	R5,	R2
				ADD		R5, R5, R3
;Logical calculations
				EOR 	R10, R6, R7
				AND		R11, R7, R9
				EOR		R11, R11, #0x13
				AND		R0, R6, R8
				ORR		R11, R11, R0
				BIC 	R11, R11, R10
;Save results
				LDR		R0, =QA
				STR		R5, [R0]
				LDR		R0, =QL
				STR		R11, [R0]
__inf_cycle
				B		__inf_cycle
				ENDP
END