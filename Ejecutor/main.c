        #include <stdio.h>
        #include <stdlib.h>
        #include "util.h"
        #include "instrucciones.h"
        #include "string.h"

        void LeerBinario(long int reg[], long int ram[], int argc, char *argv[],int imagenes,int * full);
        void Ejecucion(long int reg[], long int ram[], int flags[],char * regChar[], char * funcionesChar[]);
        void EjecucionImg(long int [], long int [],int flags[],int *error,char * muestraD[]);
        void cargaDissasembly(long int ram[],long int reg[], char * regChar[],char * funcionesChar[],char * muestraD[] , int n);
        void Interprete(long, long, long, long int [], long int [],int flags[],int * error, char * muestraD []);
        void (*funciones[0x8F])(long int *op1, long int *op2, long int reg[], long int ram[],int flags[],int * error, char * muestraD);
        void cargaOp(long int TOp, long int **Op, long celda, long int reg[], long int ram[]);
        void ejecutaOp(long int * Op1, long int * Op2, long int CodOp,long int reg[],long int ram[],int flags[],int * error, char * muestraD[]);
        void cargarFunciones(void *[]);
        void cuentaProcFlag(int *imagenes, int flags[],int argc, char*argv[]);
        void cargaFuncionesChar(char *funcionesChar[]);
        void cargaLinea(long int celda1,long int celda2, long int celda3, char * linea,int pos,int j,char * regChar[], char * funcionesChar[]);
        void chequeaTipoOP(long int celda, char * operando, long int tipoOp,char * regChar[], char * funcionesChar[]);
        void CargaRegChar(char *regChar[]);


        int main(int argc, char *argv[])
        {
            long int reg[16], ram[8192];
            void * funciones[0x8F];
            char * funcionesChar[0x8F];
            char * regChar[16];
            int flags[4]={0};
            int imagenes,full;
            cuentaProcFlag(&imagenes,flags,argc,argv);
            cargarFunciones(funciones);
            LeerBinario(reg,ram,argc,argv,imagenes,&full);
            cargaFuncionesChar(funcionesChar);
            CargaRegChar(regChar);
            if(full!=1){
                if(flags[2]==1)
                    system("cls");
                Ejecucion(reg,ram,flags,regChar,funcionesChar);
            }
            return 1;
        }

        void cuentaProcFlag(int *imagenes, int flags[],int argc, char*argv[])
        {
            int i, condicion=1, contador=0;
            argc = argc -1 ;                // el primer valor de argc no tiene nada util
            i = argc;
            while (condicion==1){
                if (strcmp(argv[i],"-d")==0){
                    flags[3]=1;
                    contador++;
                }
                else if (strcmp(argv[i],"-c")==0){
                    flags[2]=1;
                    contador++;
                }
                else if (strcmp(argv[i],"-b")==0){
                    flags[1]=1;
                    contador++;
                }
                else if (strcmp(argv[i],"-a")==0){
                    flags[0]=1;
                    condicion =0;                   // como estan en orden, si lee una -a ya terminaron las flags.
                    contador++;
                }
                else
                    condicion=0;
                i--;
            }
            *imagenes=argc-contador;

        }


        void cargarFunciones(void* func[])
        {
            funciones[0x01]=mov;
            funciones[0x02]=add;
            funciones[0x03]=sub;
            funciones[0x04]=mul;
            funciones[0x05]=divi;
            funciones[0x06]=mod;
            funciones[0x13]=cmp;
            funciones[0x17]=swap;
            funciones[0x19]=rnd;
            funciones[0x31]=and;
            funciones[0x32]=or;
            funciones[0x33]=not;
            funciones[0x34]=xor;
            funciones[0x37]=shl;
            funciones[0x38]=shr;
            funciones[0x20]=jmp;
            funciones[0x21]=je;
            funciones[0x22]=jg;
            funciones[0x23]=jl;
            funciones[0x24]=jz;
            funciones[0x25]=jp;
            funciones[0x26]=jn;
            funciones[0x27]=jnz;
            funciones[0x28]=jnp;
            funciones[0x29]=jnn;
            funciones[0x40]=call;
            funciones[0x44]=push;
            funciones[0x45]=pop;
            funciones[0x48]=ret;
            funciones[0x50]=slen;
            funciones[0x51]=smov;
            funciones[0x53]=scmp;
            funciones[0x81]=sys;
            funciones[0x8F]=stop;
        }

        void LeerBinario(long int reg[], long int ram[], int argc, char *argv[],int imagenes,int *full)
        {
            int i=0,j,corrimiento,imgactual,dataSegmentOriginal;
            *full =0;
            FILE *Arch;
            ram[0] = imagenes;
            ram[1] = 0;
            corrimiento = 16*imagenes+2;
            while(*full==0 && i<ram[0])
            {
                imgactual = i+1;                          // argv arranca de 1, por eso le sumo 1.
                Arch=fopen(argv[imgactual],"rb");
                if (Arch!=NULL)
                {
                    fread(reg, sizeof(long int), 16, Arch);     // Leo los reg de la img en los de la MV y los recalculo 1 por 1.
                    dataSegmentOriginal = reg[2];               // Guardo la longitud del CS original para copiar en ram con fread.
                    reg[1] += corrimiento;
                    reg[2] += corrimiento;
                    if(reg[3]== -1 )
                        reg[3] = ram[ (16 * (i) + 2)  + 3]; // rescato el ES de la imagen anterior. acordarse que i = imgactual -1.
                    else
                        reg[3]+= corrimiento;
                    reg[5]+= corrimiento;
                    for(j=0; j<16; j++)             // cargo los registros corregidos en su correspondiente lugar de la ram.
                        ram[16*i+2+j] = reg[j];
                    fread(&ram[reg[1]],sizeof(long int),dataSegmentOriginal,Arch);   // Cargo en Ram el CS, partiendo desde el CS corregido.
                    corrimiento += reg[0];              // al corrimiento le sumo el PS de la img actual.
                    if(corrimiento > 8192)
                        (*full) = 1;
                    fclose(Arch);
                }
                else
                {
                    printf("Archivo %d es invalido",i);
                }

                i++;
            }
            if(*full==1)
                printf("Memoria insuficiente");
        }

        void Ejecucion(long int reg[], long int ram[], int flags[],char * regChar[], char * funcionesChar[])
        {
            int i,error=0;

            if (flags[2]==1)
                system("cls");
            while(ram[1]<ram[0] && error==0)
            {
                for(i=0; i<16; i++)
                    reg[i] = ram[ram[1]*16+2 + i];
                char * muestraD[(reg[2]-reg[1])];
                if(flags[3]==1){
                    for(i=0;i<=(reg[2]-reg[1]);i++){
                        muestraD[i] = (char*) malloc(50*sizeof(char));
                        strcpy(muestraD[i],"");
                    }
                    cargaDissasembly(ram,reg,regChar,funcionesChar,muestraD,reg[2]-reg[1]); // Carga el dissasembler para mostrarlo
                    }
                EjecucionImg(reg,ram,flags,&error,muestraD);
                for(i=0; i<16; i++)
                    ram[ram[1]*16+2 + i] = reg[i];
                if(error==0)
                    ram[1]++;

            }
            if(flags[0]==1){
                printf("\n");
                printf("Cantidad total de procesos = %d \n",ram[0]);
                printf("Cantidad de procesos finalizados correctamente = %d \n",ram[1]);
                for(i=0;i<ram[1];i++){
                    printf("Proceso %d:\n",i+1);
                    printf("PS = %ld | CS = %ld | DS = %ld | ES = %ld \n",ram[16 * i +2],ram[16 * i +2+1],ram[16 * i +2+2],ram[16 * i +2+3]);
                    printf("IP = %ld | SS = %ld | SP = %ld | BP = %ld \n",ram[16 * i +2+4],ram[16 * i +2+5],ram[16 * i +2+6],ram[16 * i +2+7]);
                    printf("AC = %ld | CC = %ld | AX = %ld | BX = %ld \n",ram[16 * i +2+8],ram[16 * i +2+9],ram[16 * i +2+10],ram[16 * i +2+11]);
                    printf("CX = %ld | DX = %ld | EX = %ld | FX = %ld \n",ram[16 * i +2+12],ram[16 * i +2+13],ram[16 * i +2+14],ram[16 * i +2+15]);
                }
            }
        }


        void EjecucionImg(long int reg[], long int ram[],int flags[],int * error,char * muestraD[])
        {
            long celda1,celda2,celda3;
            long int salto;
            int cCelda=reg[1];
            reg[4]=1;
            salto = reg[4];
            while(cCelda>=reg[1] && cCelda<reg[2] && *error == 0)
            {
                celda1 = ram[cCelda];
                cCelda++;
                if((celda1 & 0xFFFF0000)==-1){
                    cCelda = reg[2]+1;
                }
                else{
                    celda2 = ram[cCelda];
                    cCelda++;
                    celda3 = ram[cCelda];
                    Interprete(celda1, celda2, celda3, reg, ram,flags,error,muestraD);
                        //printf("%s",muestraD[salto-1]);
                        //printf("\n");
                        //printf("PS = %ld | CS = %ld | DS = %ld | ES = %ld \n",reg[0],reg[1],reg[2],reg[3]);
                        //printf("IP = %ld | SS = %ld | SP = %ld | BP = %ld \n",reg[4],reg[5],reg[6],reg[7]);
                        //printf("AC = %ld | CC = %ld | AX = %ld | BX = %ld \n",reg[8],reg[9],reg[10],reg[11]);
                        //printf("CX = %ld | DX = %ld | EX = %ld | FX = %ld \n",reg[12],reg[13],reg[14],reg[15]);
                    if(salto == reg[4])
                        reg[4]++;
                    cCelda=(reg[4]-1)*3 + reg[1];
                    salto = reg[4];
                }
            }
        }

        void Interprete(long celda1, long celda2, long celda3, long int reg[], long int ram[],int flags[], int * error, char * muestraD[])
        {
            long int CodOp, TOp1, TOp2, *Op1, *Op2;
            CodOp = (celda1 & 0xFFFF0000)>>16;
            TOp1 = (celda1 & 0x0000FF00)>>8;
            TOp2 = celda1 & 0x000000FF;
            cargaOp(TOp1, &Op1, celda2, reg, ram);
            cargaOp(TOp2, &Op2, celda3, reg, ram);
            if (CodOp == 0x50)
                if (TOp2 == 0x0)
                    slen(Op1,Op2,reg,ram,flags,error,muestraD,1);
                else
                    slen(Op1,Op2,reg,ram,flags,error,muestraD,0);
            else
                if (CodOp == 0x51)
                    if (TOp2 == 0x0)
                        smov(Op1,Op2,reg,ram,flags,error,muestraD,1);
                    else
                        smov(Op1,Op2,reg,ram,flags,error,muestraD,0);
                else
                    if (CodOp == 0x53)
                        if (TOp1 == 0x0)
                            if (TOp2 == 0x0)
                                scmp(Op1,Op2,reg,ram,flags,error,muestraD,1,1);
                            else
                                scmp(Op1,Op2,reg,ram,flags,error,muestraD,1,0);
                        else
                            if (TOp2 == 0x0)
                                scmp(Op1,Op2,reg,ram,flags,error,muestraD,0,1);
                            else
                                scmp(Op1,Op2,reg,ram,flags,error,muestraD,0,0);
                    else
                        ejecutaOp(Op1,Op2,CodOp,reg,ram,flags,error,muestraD);
        }

        void cargaOp(long int TOp, long int **Op, long celda, long int reg[], long int ram[])
        {
            long aux;

            if (TOp == 0x00)
            {
                *Op=(long int)malloc(sizeof(long int));
                **Op=celda;
            }
            else
                if (TOp == 0x01)
                {
                    *Op=reg;
                    *Op+=celda;
                }
                else if (TOp == 0x02){
                    aux=(celda & 0xF0000000)>>28;
                    if (aux == 0x2)
                    {
                        aux=celda & 0x0FFFFFFF;
                        *Op=ram;
                        *Op+=reg[2]+aux;
                    }
                    else
                    {
                        if(aux == 0x3)
                        {
                            aux=celda & 0x0FFFFFFF;
                            if(aux & 0x08000000){
                                aux = aux | 0xF0000000;
                            }
                            *Op=ram;
                            *Op+=reg[3]+aux;
                        }

                    }
                }               // TOP indirecto
                else{
                    *Op=ram;
                    aux=(celda & 0xF0000000)>>28;
                    if (aux == 0x02)
                        *Op+=reg[2];
                    else if (aux == 0x03)
                        *Op+=reg[3];
                    else
                        *Op+=reg[5];
                    aux=(celda & 0x0FFFFFF0)>>4;
                    if(aux & 0x800000){
                        aux = aux | 0xFF000000;
                    }
                    *Op+=aux;
                    aux=(celda & 0x0000000F);
                    *Op+=reg[aux];
                }
        }

        void ejecutaOp(long int * Op1, long int * Op2, long int CodOp,long int reg[],long int ram[],int flags[], int * error,char * muestraD[])
        {
            (*funciones[CodOp])(Op1,Op2,reg,ram,flags,error,muestraD);

        }

        void cargaDissasembly(long int ram[],long int reg[], char * regChar[],char * funcionesChar[],char * muestraD[], int n)
        {
            int i=0,j=0,pos;
            long celda1,celda2,celda3,mascara;
            char * linea;
            linea = (char*) malloc(100 *sizeof(char));
            while(i<n)
            {
                pos = reg[1]+i;
                celda1 = ram[reg[1]+i];
                i++;
                if((celda1 & 0xFFFF0000) == 0x0)
                {
                    cargaLinea(celda1,0,0,linea,pos,j+1,regChar,funcionesChar);
                }
                else
                {
                    celda2=ram[reg[1]+i];
                    i++;
                    celda3=ram[reg[1]+i];
                    i++;
                    cargaLinea(celda1,celda2,celda3,linea,pos,j+1,regChar,funcionesChar);

                }
              strcpy(muestraD[j],linea);
              j++;
            }
            for(i=0; i<j; i++)
            {
                printf("%s \n", muestraD[i]);
            }
        }


        void cargaLinea(long int celda1,long int celda2, long int celda3, char * linea,int pos,int j,char * regChar[], char * funcionesChar[]){
            long int mnemonico,tipoOp1,tipoOp2;
            char * operando1;
            char * operando2;
            if((celda1 & 0xFFFF0000) == 0x0){
                sprintf(linea,"[%04d]: %08X \t %c",pos,celda1,celda1);
            }
            else{
                operando1= (char*) malloc(50*sizeof(char));
                operando2= (char*) malloc(50*sizeof(char));
                mnemonico = (celda1 & 0xFFFF0000) >> 16;
                tipoOp1 = (celda1 & 0x0000FF00) >> 8;
                chequeaTipoOP(celda2,operando1,tipoOp1,regChar,funcionesChar);
                if (mnemonico == 0x81 || mnemonico == 0x44 || mnemonico == 0x44 || mnemonico == 0x8F || mnemonico == 0x40 || mnemonico == 0x25|| mnemonico == 0x27){
                    sprintf(linea,"[%04d]: %08X %08X %08X    %d: %s \t %s ",pos,celda1,celda2,celda3,j,funcionesChar[mnemonico],operando1);
                }
                else{
                    tipoOp2 = (celda1 & 0x000000FF);
                    chequeaTipoOP(celda3,operando2,tipoOp2,regChar,funcionesChar);
                    sprintf(linea,"[%04d]: %08X %08X %08X    %d: %s \t %s,%s",pos,celda1,celda2,celda3,j,funcionesChar[mnemonico],operando1,operando2);
                }
            }
        }

        void chequeaTipoOP(long int celda, char * operando, long int tipoOp,char * regChar[], char * funcionesChar[]){
            long aux,aux2,aux3;

            if(tipoOp == 0x00){ // operando inmediato
                sprintf(operando,"%d",celda);
            }
            else{
                if(tipoOp == 0x01){ // operando de registro
                    aux = celda & 0x000000FF;
                    strcpy(operando,regChar[aux]);
                }
                else{
                    if(tipoOp == 0x02){ // operando directo
                        aux = (celda & 0xF0000000) >> 28;
                        aux2=celda & 0x0FFFFFFF;
                        if(aux2 & 0x08000000){
                            aux2 = aux2 | 0xF0000000;
                        }
                        if(aux == 0x00){
                            sprintf(operando,"[%s:%d]",regChar[2],aux2);
                        }
                        else{
                            sprintf(operando,"[%s:%d]",regChar[aux],aux2);
                        }
                    }
                    else{
                        if(tipoOp == 0x03){ // operando indirecto
                            aux = (celda & 0xF0000000) >> 28;
                            aux2 = (celda & 0x0000000F);
                            aux3 = (celda & 0x0FFFFFF0)>>4;
                            if(aux3 & 0x800000){
                                aux3 = aux3 | 0xFF000000;
                            }
                            sprintf(operando,"[%s:%s + %d]",regChar[aux],regChar[aux2],aux3);
                        }
                    }
                }
            }
        }


        void CargaRegChar(char *regChar[])
        {
            int i;
            for(i=0;i<=16;i++){
                regChar[i] = (char*) malloc(5*sizeof(char));
                strcpy(regChar[i],"");
            }
            strcpy(regChar[0],"PS");
            strcpy(regChar[1],"CS");
            strcpy(regChar[2],"DS");
            strcpy(regChar[3],"ES");
            strcpy(regChar[4],"IP");
            strcpy(regChar[5],"SS");
            strcpy(regChar[6],"SP");
            strcpy(regChar[7],"BP");
            strcpy(regChar[8],"AC");
            strcpy(regChar[9],"CC");
            strcpy(regChar[10],"AX");
            strcpy(regChar[11],"BX");
            strcpy(regChar[12],"CX");
            strcpy(regChar[13],"DX");
            strcpy(regChar[14],"EX");
            strcpy(regChar[15],"FX");
        }

        void cargaFuncionesChar(char *funcionesChar[]){
            int i;
            for (i=0; i<=0x8F; i++)
            {
                funcionesChar[i] = (char*) malloc(5*sizeof(char));
                strcpy(funcionesChar[i],"");
            }
            strcpy(funcionesChar[0x01],"MOV");
            strcpy(funcionesChar[0x02],"ADD");
            strcpy(funcionesChar[0x03],"SUB");
            strcpy(funcionesChar[0x04],"MUL");
            strcpy(funcionesChar[0x05],"DIVI");
            strcpy(funcionesChar[0x06],"MOD");
            strcpy(funcionesChar[0x13],"CMP");
            strcpy(funcionesChar[0x17],"SWAP");
            strcpy(funcionesChar[0x19],"RND");
            strcpy(funcionesChar[0x31],"AND");
            strcpy(funcionesChar[0x32],"OR");
            strcpy(funcionesChar[0x33],"NOT");
            strcpy(funcionesChar[0x34],"XOR");
            strcpy(funcionesChar[0x37],"SHL");
            strcpy(funcionesChar[0x38],"SHR");
            strcpy(funcionesChar[0x20],"JMP");
            strcpy(funcionesChar[0x21],"JE");
            strcpy(funcionesChar[0x22],"JG");
            strcpy(funcionesChar[0x23],"JL");
            strcpy(funcionesChar[0x24],"JZ");
            strcpy(funcionesChar[0x25],"JP");
            strcpy(funcionesChar[0x26],"JN");
            strcpy(funcionesChar[0x27],"JNZ");
            strcpy(funcionesChar[0x28],"JNP");
            strcpy(funcionesChar[0x29],"JNN");
            strcpy(funcionesChar[0x40],"CALL");
            strcpy(funcionesChar[0x44],"PUSH");
            strcpy(funcionesChar[0x45],"POP");
            strcpy(funcionesChar[0x48],"RET");
            strcpy(funcionesChar[0x50],"SLEN");
            strcpy(funcionesChar[0x51],"SMOV");
            strcpy(funcionesChar[0x53],"SCMP");
            strcpy(funcionesChar[0x81],"SYS");
            strcpy(funcionesChar[0x8F],"STOP");

        }
