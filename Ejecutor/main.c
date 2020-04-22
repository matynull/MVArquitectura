#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include "instrucciones.h"

void LeerBinario(long int [], long int [], int argc, char *argv[]);
void Ejecucion(long int [], long int []);
void Interprete(long, long, long, long int [], long int []);
void (*funciones[0x8F])(long int *op1, long int *op2, long int reg[], long int ram[]);
void cargaOp(long int TOp, long int **Op, long celda, long int reg[], long int ram[]);
void ejecutaOp(long int * Op1, long int * Op2, long int CodOp,long int reg[],long int ram[]);
void cargarFunciones(void *[]);

int main(int argc, char *argv[])
{
    long int reg[16], ram[2000];
    void * funciones[0x8F];
    cargarFunciones(funciones);
    LeerBinario(reg,ram,argc,argv);
    Ejecucion(reg,ram);
    return 0;
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
    funciones[0x81]=sys;
    funciones[0x8F]=stop;
}

void LeerBinario(long int reg[], long int ram[], int argc, char *argv[])
{
    int i;
    FILE *Arch;
    Arch=fopen("imagenMemoria.img","rb");
    //Arch=fopen(argv[1],"rb");
    if (Arch!=NULL)
    {
        fread(reg, sizeof(long int), 16, Arch);
        fread(ram, sizeof(long int), 2000, Arch);
        fclose(Arch);
        if (argc>2 && strcmp(argv[2],"-d") == 0)
            for (i=0; i<reg[2]; i+=3)
                printf("[%08X]\t\t\t%08X %08X %08X\n",i,ram[i],ram[i+1],ram[i+2]);
    }
    else
        printf("No se encontro el archivo");
    fclose(Arch);
}



void Ejecucion(long int reg[], long int ram[])
{
    long celda1,celda2,celda3;
    long int salto;
    int cCelda=0;
    reg[4]=1;
    salto = reg[4];
    while(cCelda>=0 && cCelda<reg[2])
    {
        celda1 = ram[cCelda];
        cCelda++;
        celda2 = ram[cCelda];
        cCelda++;
        celda3 = ram[cCelda];
        Interprete(celda1, celda2, celda3, reg, ram);
        if(salto == reg[4])
            reg[4]++;
        cCelda=(reg[4]-1)*3;
        salto = reg[4];
        ram[reg[2]+reg[13]];
    }
}

void Interprete(long celda1, long celda2, long celda3, long int reg[], long int ram[])
{
    long int CodOp, TOp1, TOp2, *Op1, *Op2;
    CodOp = (celda1 & 0xFFFF0000)>>16;
    TOp1 = (celda1 & 0x0000FF00)>>8;
    TOp2 = celda1 & 0x000000FF;
    cargaOp(TOp1, &Op1, celda2, reg, ram);
    cargaOp(TOp2, &Op2, celda3, reg, ram);
    ejecutaOp(Op1,Op2,CodOp,reg,ram);
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
        else
        {
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
                    *Op=ram;
                    *Op+=reg[3]+aux;
                }

            }
        }
}

void ejecutaOp(long int * Op1, long int * Op2, long int CodOp,long int reg[],long int ram[])
{
    (*funciones[CodOp])(Op1,Op2,reg,ram);
}
