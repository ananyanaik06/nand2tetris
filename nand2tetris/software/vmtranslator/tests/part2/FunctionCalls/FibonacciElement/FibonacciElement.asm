// Bootstrap code
@256
D = A
@SP
M = D
@RET_0
D = A
@SP
M = M + 1
A = M - 1
M = D
@LCL
D = M
@SP
M = M + 1
A = M - 1
M = D
@ARG
D = M
@SP
M = M + 1
A = M - 1
M = D
@THIS
D = M
@SP
M = M + 1
A = M - 1
M = D
@THAT
D = M
@SP
M = M + 1
A = M - 1
M = D
@SP
D = M
@LCL
M = D
@0
D = A
@5
D = D + A
@SP
D = M - D
@ARG
M = D
@Sys.init
0; JMP
(RET_0)
//function Main.fibonacci 0
(Main.fibonacci)
//push argument 0
@ARG
D = M
@0
A = D + A
D = M
@SP
M = M + 1
A = M - 1
M = D
//push constant 2
@2
D = A
@SP
M = M + 1
A = M - 1
M = D
//lt                     
@SP
M = M - 1
A = M
D = M
A = A - 1
D = M - D
@TRUE0
D; JLT
@SP
A = M - 1
M = 0
@END0
0; JMP
(TRUE0)
@SP
A = M - 1
M = -1
(END0)
//if-goto N_LT_2        
@SP
AM = M - 1
D = M
@N_LT_2
D; JNE
//goto N_GE_2
@N_GE_2
0; JMP
//label N_LT_2               
(N_LT_2)
//push argument 0        
@ARG
D = M
@0
A = D + A
D = M
@SP
M = M + 1
A = M - 1
M = D
//return
@LCL
D = M
@R13
M = D
@5
A = D - A
D = M
@R14
M = D
@SP
A = M - 1
D = M
@ARG
A = M
M = D
@ARG
D = M + 1
@SP
M = D
@R13
AM = M - 1
D = M
@THAT
M = D
@R13
AM = M - 1
D = M
@THIS
M = D
@R13
AM = M - 1
D = M
@ARG
M = D
@R13
AM = M - 1
D = M
@LCL
M = D
@R14
A = M
0; JMP
//label N_GE_2               
(N_GE_2)
//push argument 0
@ARG
D = M
@0
A = D + A
D = M
@SP
M = M + 1
A = M - 1
M = D
//push constant 2
@2
D = A
@SP
M = M + 1
A = M - 1
M = D
//sub
@SP
M = M - 1
A = M
D = M
A = A - 1
M = M - D
//call Main.fibonacci 1  
@RET_1
D = A
@SP
M = M + 1
A = M - 1
M = D
@LCL
D = M
@SP
M = M + 1
A = M - 1
M = D
@ARG
D = M
@SP
M = M + 1
A = M - 1
M = D
@THIS
D = M
@SP
M = M + 1
A = M - 1
M = D
@THAT
D = M
@SP
M = M + 1
A = M - 1
M = D
@SP
D = M
@LCL
M = D
@1
D = A
@5
D = D + A
@SP
D = M - D
@ARG
M = D
@Main.fibonacci
0; JMP
(RET_1)
//push argument 0
@ARG
D = M
@0
A = D + A
D = M
@SP
M = M + 1
A = M - 1
M = D
//push constant 1
@1
D = A
@SP
M = M + 1
A = M - 1
M = D
//sub
@SP
M = M - 1
A = M
D = M
A = A - 1
M = M - D
//call Main.fibonacci 1  
@RET_2
D = A
@SP
M = M + 1
A = M - 1
M = D
@LCL
D = M
@SP
M = M + 1
A = M - 1
M = D
@ARG
D = M
@SP
M = M + 1
A = M - 1
M = D
@THIS
D = M
@SP
M = M + 1
A = M - 1
M = D
@THAT
D = M
@SP
M = M + 1
A = M - 1
M = D
@SP
D = M
@LCL
M = D
@1
D = A
@5
D = D + A
@SP
D = M - D
@ARG
M = D
@Main.fibonacci
0; JMP
(RET_2)
//add                    
@SP
M = M - 1
A = M
D = M
A = A - 1
M = D + M
//return
@LCL
D = M
@R13
M = D
@5
A = D - A
D = M
@R14
M = D
@SP
A = M - 1
D = M
@ARG
A = M
M = D
@ARG
D = M + 1
@SP
M = D
@R13
AM = M - 1
D = M
@THAT
M = D
@R13
AM = M - 1
D = M
@THIS
M = D
@R13
AM = M - 1
D = M
@ARG
M = D
@R13
AM = M - 1
D = M
@LCL
M = D
@R14
A = M
0; JMP
//function Sys.init 0
(Sys.init)
//    
//push constant 4
@4
D = A
@SP
M = M + 1
A = M - 1
M = D
//call Main.fibonacci 1
@RET_3
D = A
@SP
M = M + 1
A = M - 1
M = D
@LCL
D = M
@SP
M = M + 1
A = M - 1
M = D
@ARG
D = M
@SP
M = M + 1
A = M - 1
M = D
@THIS
D = M
@SP
M = M + 1
A = M - 1
M = D
@THAT
D = M
@SP
M = M + 1
A = M - 1
M = D
@SP
D = M
@LCL
M = D
@1
D = A
@5
D = D + A
@SP
D = M - D
@ARG
M = D
@Main.fibonacci
0; JMP
(RET_3)
//label END  
(END)
//goto END  
@END
0; JMP
(END)
@END
0; JMP
