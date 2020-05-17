
\\ASM EXTRA =550  	STACK= 700    //Directiva

	MOV AX,5
   cON1   EQU 'A
	CoN2 EQU    "Hola como andas"
	MOV FX ,  	CoN1 //Guarda en FX la constante CON1
	MOV [ES:AX-1] , %0001 //Prueba si lo indirectos andan bien

SLEN [DS:10], con2 //Guarda la longitud del string CON2 en la celda DS:10

	MOV AX,[ES:4]
	MOV CX,1	
	MOV DX, 10
	SYS 2 //Imprime la longitud del string CON2

	SMOV [ES:7]  ,  CON2
	MOV AX,%0100
	MOV BX,3
	MOV DX,7
	SYS 20 //Imprime la cadena copiada con un endline

	SCMP CON2 , [ES:7]
	JNZ fin

loop:	MOV AX,0
	MOV BX , 2
	MOV DX,100
	SYS 10 //Lee un string por teclado y lo guarda a partir de la celda [DS:100]

	SCMP [DS:100],[ES:7]
	JNZ loop
fin:	STOP

	
	
	