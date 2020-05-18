    #include <stdio.h>
    #include <stdlib.h>
    #include <time.h>
    #include "instrucciones.h"



    void push(long int *op1, long int *op2, long int reg[], long int ram[], int flags[], int *error, char * muestraD[])
    {
        reg[6]--;
        if(reg[6]>0)
            ram[reg[5]+reg[6]] = *op1;
        else
        {
            reg[4]=-1;
            *error =1;
            printf("Stack Overflow\n");
        }
    }


    void pop(long int *op1, long int *op2, long int reg[], long int ram[], int flags[], int *error, char * muestraD[])
    {
        if(reg[6]+reg[5]< reg[1] + reg[0])
        {
            *op1 = ram[reg[5]+ reg[6]];
            reg[6]++;
        }
        else
        {
            reg[4]=-1;
            *error =1;
            printf("Stack Underflow\n");
        }
    }

    void call(long int *op1, long int *op2, long int reg[], long int ram[], int flags[], int *error, char * muestraD[])
    {
        reg[6]--;
        if(reg[6]>0){
            ram[reg[5]+reg[6]] = reg[4]+1;
            reg[4] = *op1;
        }
        else
        {
            reg[4]=-1;
            *error =1;
            printf("Stack Overflow\n");
        }
    }

    void ret(long int *op1, long int *op2, long int reg[], long int ram[], int flags[], int *error, char * muestraD[])
    {
        if(reg[6]+reg[5]< reg[1] + reg[0])
        {
            reg[4] = ram[reg[5] + reg[6]];
            reg[6]++;
        }
        else
        {
            reg[4]=-1;
            *error =1;
            printf("Stack Underflow\n");
        }
    }


    void mov(long int *op1, long int *op2, long int reg[], long int ram[], int flags[], int *error, char * muestraD[])
    {
        *op1=*op2;
        reg[9]=0x00000000;
        if (*op1 == 0)
            (reg[9])+=1;
        else
            if (*op1 < 0)
                (reg[9])+=0x80000000;
    }

    void add(long int *op1, long int *op2, long int reg[], long int ram[], int flags[], int *error, char * muestraD[])
    {
        *op1=(*op1) + (*op2);
        reg[9]=0x00000000;
        if (*op1 == 0)
            (reg[9])+=1;
        else
            if (*op1 < 0)
                (reg[9])+=0x80000000;
    }

    void sub(long int *op1, long int *op2, long int reg[], long int ram[], int flags[], int *error, char * muestraD[])
    {
        *op1=(*op1) - (*op2);
        reg[9]=0x00000000;
        if (*op1 == 0)
            (reg[9])+=1;
        else
            if (*op1 < 0)
                (reg[9])+=0x80000000;
    }

    void mul(long int *op1, long int *op2, long int reg[], long int ram[], int flags[], int *error, char * muestraD[])
    {
        *op1=(*op1) * (*op2);
        reg[9]=0x00000000;
        if (*op1 == 0)
            (reg[9])+=1;
        else
            if (*op1 < 0)
                (reg[9])+=0x80000000;
    }

    void divi(long int *op1, long int *op2, long int reg[], long int ram[], int flags[], int *error, char * muestraD[])
    {
        *op1=(*op1) / (*op2);
        reg[9]=0x00000000;
        if (*op1 == 0)
            (reg[9])+=1;
        else
            if (*op1 < 0)
                (reg[9])+=0x80000000;
    }

    void mod(long int *op1, long int *op2, long int reg[], long int ram[], int flags[], int *error, char * muestraD[])
    {
        *op1=(*op1)%(*op2);
        reg[9]=0x00000000;
        if (*op1 == 0)
            (reg[9])+=1;
        else
            if (*op1 < 0)
                (reg[9])+=0x80000000;
    }

    void cmp(long int *op1, long int *op2, long int reg[], long int ram[], int flags[], int *error, char * muestraD[])
    {
        reg[9]=0x00000000;
        if ((*op1) - (*op2) == 0)
            (reg[9])+=1;
        else
            if ((*op1) - (*op2) < 0)
                (reg[9])+=0x80000000;
    }

    void swap(long int *op1, long int *op2, long int reg[], long int ram[], int flags[], int *error, char * muestraD[])
    {
        long int aux;

        aux=*op1;
        *op1=*op2;
        *op2=aux;
    }

    void rnd(long int *op1, long int *op2, long int reg[], long int ram[], int flags[], int *error, char * muestraD[])
    {
        time_t t;

        srand((unsigned) time(&t));
        *op1=rand()%(*op2);
    }

    void and(long int *op1, long int *op2, long int reg[], long int ram[], int flags[], int *error, char * muestraD[])
    {
        *op1=(*op1) & (*op2);
        reg[9]=0x00000000;
        if ((*op1)== 0)
            (reg[9])+=1;
        else
            if ((*op1)< 0)
                (reg[9])+=0x80000000;
    }

    void or(long int *op1, long int *op2, long int reg[], long int ram[], int flags[], int *error, char * muestraD[])
    {
        *op1=(*op1) | (*op2);
        reg[9]=0x00000000;
        if ((*op1) == 0)
            (reg[9])+=1;
        else
            if ((*op1)< 0)
                (reg[9])+=0x80000000;
    }

    void not(long int *op1, long int *op2, long int reg[], long int ram[], int flags[], int *error, char * muestraD[])
    {
        *op1=~(*op1);
        reg[9]=0x00000000;
        if ((*op1) == 0)
            (reg[9])+=1;
        else
            if ((*op1) < 0)
                (reg[9])+=0x80000000;
    }

    void xor(long int *op1, long int *op2, long int reg[], long int ram[], int flags[], int *error, char * muestraD[])
    {
        *op1=(*op1) ^ (*op2);
        reg[9]=0x00000000;
        if ((*op1) == 0)
            (reg[9])+=1;
        else
            if ((*op1) < 0)
                (reg[9])+=0x80000000;
    }

    void shl(long int *op1, long int *op2, long int reg[], long int ram[], int flags[], int *error, char * muestraD[])
    {
        *op1=(*op1) << (*op2);
    }

    void shr(long int *op1, long int *op2, long int reg[], long int ram[], int flags[], int *error, char * muestraD[])
    {
        *op1=(*op1) >> (*op2);
        *op1 = *op1 & 0x7FFFFFFF;
    }


    void jmp(long int *op1, long int *op2, long int reg[], long int ram[], int flags[], int *error, char * muestraD[])
    {
        reg[4]=*op1;
    }

    void je(long int *op1, long int *op2, long int reg[], long int ram[], int flags[], int *error, char * muestraD[])
    {
        if ((*op1)==reg[0x0A])
            reg[4]=*op2;
    }

    void jg(long int *op1, long int *op2, long int reg[], long int ram[], int flags[], int *error, char * muestraD[])
    {
        if ((*op1)>reg[0x0A])
            reg[4]=*op2;
    }

    void jl(long int *op1, long int *op2, long int reg[], long int ram[], int flags[], int *error, char * muestraD[])
    {
        if ((*op1)<reg[0x0A])
            reg[4]=*op2;
    }

    void jz(long int *op1, long int *op2, long int reg[], long int ram[], int flags[], int *error, char * muestraD[])
    {
        if ((reg[9])==0x01)
            reg[4]=*op1;
    }

    void jn(long int *op1, long int *op2, long int reg[], long int ram[], int flags[], int *error, char * muestraD[])
    {
        if ((reg[9])==0x80000000)
            reg[4]=*op1;
    }

    void jp(long int *op1, long int *op2, long int reg[], long int ram[], int flags[], int *error, char * muestraD[])
    {
        if (((reg[9])!=0x80000000) && ((reg[9])!=0x01))
            reg[4]=*op1;
    }

    void jnz(long int *op1, long int *op2, long int reg[], long int ram[], int flags[], int *error, char * muestraD[])
    {
        if ((reg[9])!=0x01)
            reg[4]=*op1;
    }

    void jnn(long int *op1, long int *op2, long int reg[], long int ram[], int flags[], int *error, char * muestraD[])
    {
        if ((reg[9])!=0x80000000)
            reg[4]=*op1;
    }

    void jnp(long int *op1, long int *op2, long int reg[], long int ram[], int flags[], int *error, char * muestraD[])
    {
        if (((reg[9])==0x80000000) || ((reg[9])==0x01))
            reg[4]=*op1;
    }

    void slen(long int *op1, long int *op2, long int reg[], long int ram[], int flags[], int *error, char * muestraD[],int inmediato2)
    {
        *op1=0;
        int i;
        if(inmediato2 == 1){
            i=*op2+reg[1];
            while(ram[i]!=0x00){
                (*op1)++;
                i++;
            }
        }
        else{
            while (*op2 != 0x00){
            (*op1)++;
            op2++;
            }
        }

    }

    void smov(long int *op1, long int *op2, long int reg[], long int ram[], int flags[], int *error, char * muestraD[],int inmediato2)
    {
        int i;
        if(inmediato2== 1){
            i=*op2 +reg[1];
            while(ram[i] != 0x00){
                *op1 = ram[i];
                i++;
                op1++;
            }
        }
        else{
            while( *op2 != 0x00){
                *op1=*op2;
                op2++;
                op1++;
            }
        }

    }
    void scmp(long int *op1, long int *op2, long int reg[], long int ram[], int flags[], int *error, char * muestraD[],int inmediato1, int inmediato2)
    {
        int i=*op1, j=*op2;
        long aux1,aux2;
        reg[9]=0x00;
        if(inmediato1 == 1){
            if(inmediato2==1){      // primero y 2do inmediatos
                i= *op1 +reg[1];
                j= *op2 + reg[1];
                while ((ram[i] != 0x00 || ram[j]!=0x00) && (ram[i]==ram[j])){
                    i++;
                    j++;
                }
                aux1=ram[i];
                aux2=ram[j];
            }
            else{               // solo primero inmediato
                i = *op1 + reg[1];
                while ((ram[i] != 0x00 || *op2!=0x00) && (ram[i]==*op2)){
                    i++;
                    op2++;
                }
                aux1 = ram[i];
                aux2 = *op2;

            }
        }
        else{
            if(inmediato2==1){      // solo 2do inmediato
                j=*op2+reg[1];
                while ((*op1!= 0x00 || ram[j]!=0x00) && (*op1==ram[j])){
                    op1++;
                    j++;
                }
                aux1 = *op1;
                aux2 = ram[j];
            }
            else{       // ninguno inmediato
                while ((*op1!= 0x00 || *op2!=0x00) && (*op1==*op2)){
                    op1++;
                    op2++;
                }
                aux1=*op1;
                aux2=*op2;
            }
        }
        if(aux1==aux2){
            reg[9]+=0x01;
        }
        if (aux1<aux2)
            reg[9]=0x80000000;
    }

void sys(long int *op1, long int *op2, long int reg[], long int ram[], int flags[], int *error, char * muestraD[])
{
    long mascara=0x01, aux;
    int bits[16], i, v1, v2,j;
    char auxc;
    char respuesta[10];
    char * token;

    for (i=0; i<16; i++)
    {
        if (reg[10] & mascara)
            bits[i]=1;
        else
            bits[i]=0;
        mascara <<= 1;
    }
    if (*op1 == 1)              // ES DE TIPO LECTURA
    {
        if (bits[8] == 0)       // ES DE TIPO INTERPRETAR CONTENIDO
        {
            if(bits[12]==0)     // ESCRIBE PROMPT
            {
                if (bits[0] == 1)   // ES DECIMAL
                {
                    for (i=0; i<reg[12]; i++)
                    {
                        printf("[%04d]:",(reg[13]+i));
                        scanf("%d",&aux);
                        ram[reg[13]+reg[2]+i]=aux;
                    }
                }
                else if (bits[2] == 1)               // ES OCTAL
                {
                    for (i=0; i<reg[12]; i++)
                    {
                        printf("[%04d]:",(reg[13]+i));
                        scanf("%o",&aux);
                        ram[reg[13]+reg[2]+i]=aux;
                    }
                }
                else                               // ES HEXA
                {
                    for (i=0; i<reg[12]; i++)
                    {
                        printf("[%04d]:",(reg[13]+i));
                        scanf("%x",&aux);
                        ram[reg[13]+reg[2]+i]=aux;
                    }
                }
            }
            else                                // NO ESCRIBE PROMPT
            {
                if (bits[0] == 1)
                {
                    for (i=0; i<reg[12]; i++)
                    {
                        scanf("%d",&aux);
                        ram[reg[13]+reg[2]+i]=aux;
                    }
                }
                else if (bits[2] == 1)
                {
                    for (i=0; i<reg[12]; i++)
                    {
                        scanf("%o",&aux);
                        ram[reg[13]+reg[2]+i]=aux;
                    }
                }
                else
                {
                    for (i=0; i<reg[12]; i++)
                    {
                        scanf("%x",&aux);
                        ram[reg[13]+reg[2]+i]=aux;
                    }
                }
            }
        }
        else                       // LEE CHARACTER
        {
            if(bits[12]==0)        // ESCRIBE PROMPT
            {
                printf("[%04d]:",(reg[13]));
            }
            i=0;
            fflush(stdin);
            do
            {
                scanf("%c",&auxc);
                if(auxc!=0x0A)
                    ram[reg[13]+reg[2]+i]=auxc;
                i++;
            }
            while ((auxc!=0x0A) && (i<reg[12]));
            if(auxc == 0x0A)
            {
                ram[reg[13]+reg[2]+i-1] = "\0";
            }
        }

    }
    else if ((*op1 == 2) || (*op1 == 3))
    {
        if(bits[12]==0)
        {
            for(i=0; i<reg[12]; i++)
            {
                printf("[%04ld]:",(reg[13]+i));
                if(bits[4]==1)  // IMPRIME CELDA CHAR
                {
                    mascara=0xFF;
                    auxc=(char)(ram[reg[2]+reg[13]+i] & mascara);
                    if(isalpha(auxc))
                        printf(" %c",auxc);
                    else
                        printf(".");
                }
                if(bits[3]==1)
                {
                    printf(" %%%X",ram[reg[2]+reg[13]+i]);
                }
                if(bits[2]==1)
                {
                    printf(" @%o",ram[reg[2]+reg[13]+i]);
                }
                if(bits[0]==1)
                {
                    printf(" %d",ram[reg[2]+reg[13]+i]);
                }
                if(bits[8]==0)
                {
                    printf("\n");
                }
            }
        }
        else
        {
            for(i=0; i<reg[12]; i++)
            {
                if(bits[4]==1)  // IMPRIME CELDA CHAR
                {
                    mascara=0xFF;
                    auxc=(char)(ram[reg[2]+reg[13]+i] & mascara);
                    printf(" %c",auxc);
                }
                if(bits[3]==1)
                {
                    printf(" %%%X",ram[reg[2]+reg[13]+i]);
                }
                if(bits[2]==1)
                {
                    printf(" @%o",ram[reg[2]+reg[13]+i]);
                }
                if(bits[0]==1)
                {
                    printf(" %d",ram[reg[2]+reg[13]+i]);
                }
                if(bits[8]==1)
                {
                    printf("\n");
                }

            }
        }
        if (*op1 == 3)
        {
            printf("\n");
            if (bits[0] == 1)
            {
                printf("[DS]: %d\n",reg[2]);
                printf("[ES]: %d\n",reg[3]);
                printf("[IP]: %d\n",reg[4]);
                printf("[AC]: %d\n",reg[8]);
                printf("[CC]: %d\n",reg[9]);
                printf("[AX]: %d\n",reg[10]);
                printf("[BX]: %d\n",reg[11]);
                printf("[CX]: %d\n",reg[12]);
                printf("[DX]: %d\n",reg[13]);
                printf("[EX]: %d\n",reg[14]);
                printf("[FX]: %d\n",reg[15]);
            }
            else if (bits[2] == 1)
            {
                printf("[DS]: %o\n",reg[2]);
                printf("[ES]: %o\n",reg[3]);
                printf("[IP]: %o\n",reg[4]);
                printf("[AC]: %o\n",reg[8]);
                printf("[CC]: %o\n",reg[9]);
                printf("[AX]: %o\n",reg[10]);
                printf("[BX]: %o\n",reg[11]);
                printf("[CX]: %o\n",reg[12]);
                printf("[DX]: %o\n",reg[13]);
                printf("[EX]: %o\n",reg[14]);
                printf("[FX]: %o\n",reg[15]);
            }
            else if (bits[3] == 1)
            {
                printf("[DS]: %04X\n",reg[2]);
                printf("[ES]: %04X\n",reg[3]);
                printf("[IP]: %04X\n",reg[4]);
                printf("[AC]: %04X\n",reg[8]);
                printf("[CC]: %04X\n",reg[9]);
                printf("[AX]: %04X\n",reg[10]);
                printf("[BX]: %04X\n",reg[11]);
                printf("[CX]: %04X\n",reg[12]);
                printf("[DX]: %04X\n",reg[13]);
                printf("[EX]: %04X\n",reg[14]);
                printf("[FX]: %04X\n",reg[15]);
            }
            else if (bits[4] == 1)
            {
                mascara=0xFF;
                if(isalpha((char)(reg[2] & mascara)))
                    printf("[DS]: %c\n",(char)(reg[2] & mascara));
                else
                    printf("[DS]: .\n");
                if(isalpha((char)(reg[3] & mascara)))
                    printf("[ES]: %c\n",(char)(reg[3] & mascara));
                else
                    printf("[ES]: .\n");
                if(isalpha((char)(reg[4] & mascara)))
                    printf("[IP]: %c\n",(char)(reg[4] & mascara));
                else
                    printf("[IP]: .\n");
                if(isalpha((char)(reg[8] & mascara)))
                    printf("[AC]: %c\n",(char)(reg[8] & mascara));
                else
                    printf("[AC]: .\n");
                if(isalpha((char)(reg[9] & mascara)))
                    printf("[CC]: %c\n",(char)(reg[9] & mascara));
                else
                    printf("[CC]: .\n");
                if(isalpha((char)(reg[10] & mascara)))
                    printf("[AX]: %c\n",(char)(reg[10] & mascara));
                else
                    printf("[AX]: %c\n");
                if(isalpha((char)(reg[11] & mascara)))
                    printf("[BX]: %c\n",(char)(reg[11] & mascara));
                else
                    printf("[BX]: .\n");
                if(isalpha((char)(reg[12] & mascara)))
                    printf("[CX]: %c\n",(char)(reg[12] & mascara));
                else
                    printf("[CX]: .\n");
                if(isalpha((char)(reg[13] & mascara)))
                    printf("[DX]: %c\n",(char)(reg[13] & mascara));
                else
                    printf("[DX]: .\n");
                if(isalpha((char)(reg[14] & mascara)))
                    printf("[EX]: %c\n",(char)(reg[14] & mascara));
                else
                    printf("[EX]: .\n");
                if(isalpha((char)(reg[15] & mascara)))
                    printf("[FX]: %c\n",(char)(reg[15] & mascara));
                else
                    printf("[FX]: .\n");
            }
            printf("\n");
        }

    }
    else if(*op1 == 0)      // SYS 0
    {
        if (flags[2]==1)            // -c
            system("cls");
        if(flags[1]==1)             // -b
        {
            printf("[%03d] cmd:",reg[4]);
            char delim[]=" \t\n";
            char caracter;
            fflush(stdin);
            fgets(respuesta,10,stdin);
            i=0;
            token = strtok(respuesta,delim);
            if (token != NULL)
            {
                v1=atoi(token);
                token = strtok(NULL,delim);
                if (token != NULL)
                    v2=atoi(token);
                else
                    v2=v1;
                for (i=v1; i<=v2; i++)
                {
                    if (ram[i]>=0x20 && ram[i]<=0x7E)
                        caracter=ram[i];
                    else
                        caracter='.';
                    printf("[%04d]: %08X %c %ld\n",i,ram[i],caracter,ram[i]);
                }
            }
        }

        if (flags[3]==1)            //-d
        {
            char fin='0';
            fflush(stdin);
            while(fin!='\n'){
                i=1;
                int aux3 = strlen(muestraD[i]);
                while(aux3 > 32){
                if(i!=reg[4]){
                        printf("   %s\n",muestraD[i-1]);
                    }
                    if(i==reg[4]){
                        printf(">> %s\n",muestraD[i-1]);
                    }
                    i++;
                    aux3 = strlen(muestraD[i-1]);
                }
                i=0;
                printf("Registros:\n");
                printf("PS = %ld | CS = %ld | DS = %ld | ES = %ld \n",ram[16 * i +2],ram[16 * i +2+1],ram[16 * i +2+2],ram[16 * i +2+3]);
                printf("IP = %ld | SS = %ld | SP = %ld | BP = %ld \n",ram[16 * i +2+4],ram[16 * i +2+5],ram[16 * i +2+6],ram[16 * i +2+7]);
                printf("AC = %ld | CC = %ld | AX = %ld | BX = %ld \n",ram[16 * i +2+8],ram[16 * i +2+9],ram[16 * i +2+10],ram[16 * i +2+11]);
                printf("CX = %ld | DX = %ld | EX = %ld | FX = %ld \n",ram[16 * i +2+12],ram[16 * i +2+13],ram[16 * i +2+14],ram[16 * i +2+15]);
                fflush(stdin);
                scanf("%c",&fin);
            }
        }
    }
    else
    {
        if(*op1 == 10)     // SYS 10
        {
            char auxiliar[18];
            if(reg[11] == 0x2)  // lee en el DS
            {

                i=0;
                if(bits[12]==0)  // escribe prompt
                    {
                        printf("[%04d]:",(reg[13]+reg[3]));
                    }
                fflush(stdin);
                gets(auxiliar);
                while(auxiliar[i]!=0x00)
                {
                    ram[reg[2]+reg[13]+i] = auxiliar[i];
                    i++;
                }
                ram[reg[2]+reg[13]+i] = 0x00;
            }
            else
            {
                if(reg[11]==0x3) // lee en el ES
                {
                    i=0;
                    if(bits[12]==0)  // escribe prompt
                    {
                        printf("[%04d]:",(reg[13]+reg[3]));
                    }
                    i=0;
                    fflush(stdin);
                    gets(auxiliar);
                    while(auxiliar[i]!=0x00)
                    {
                        ram[reg[3]+reg[13]+i] = auxiliar[i];
                        i++;
                    }
                    ram[reg[3]+reg[13]+i] =0x00;
                }
            }


        }
        else
        {
            if(*op1 == 20)       // SYS 20
            {
                if(reg[11]==0x1)        // LEE DEL CS
                {
                    if(bits[12]==0)         // ESCRIBO PROMPT
                    {
                        printf("[%04d]:",(reg[1]+reg[13]));
                    }
                    i=0;
                    auxc=99;
                    while(auxc!=0x00)
                    {
                        auxc=ram[reg[1]+reg[13]+i];
                        printf("%c",auxc);
                        i++;
                    }
                }
                else
                {
                    if(reg[11]==0x2)        //LEE DEL DS
                    {
                        if(bits[12]==0)
                        {
                            printf("[%04d]:",(reg[13]+reg[2]));
                        }
                        i=0;
                        auxc=99;
                        while(auxc!=0x00)
                        {
                            auxc=ram[reg[2]+reg[13]+i];
                            printf("%c",auxc);
                            i++;
                        }
                    }
                    else
                    {
                        if(reg[11]==0x3)        // LEE DEL ES
                        {
                            if(bits[12]==0)
                            {
                                printf("[%04d]:",(reg[13]+reg[3]));
                            }
                            i=0;
                            auxc=99;
                            while(auxc!=0x00)
                            {
                                auxc = ram[reg[3]+reg[13]+i];
                                printf("%c",auxc);
                                i++;
                            }
                        }
                    }
                }
                if(bits[8]==0)          // ESCRIBO ENDLINE
                    printf("\n");
            }

        }
    }

}

    void stop(long int *op1, long int *op2, long int reg[], long int ram[], int flags[], int *error, char * muestraD[])
    {
        reg[4]=-1;
    }
