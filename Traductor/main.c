#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "util.h"

typedef struct
{
    char mnemonico[5], arg1[10], arg2[10];
} TInstruccionTexto;

void generarTraduccion(char* instrucciones[], int* contInstrucciones);

void PrimeraPasada(FILE* archEnt, TRotulo rotulos[], int* contRotulos, TInstruccionTexto instrucciones[], int* contLinea);

void MostrarRotulos(int cant, TRotulo rotulos[]);

void argumentoGenerico(TRam ram, int i, char argumento[], TRotulo rotulos[], int contRotulos, int* huboError, int numArg);

long traducirMnemonico(char* instrucciones[], int contInstrucciones, char mnemonico[]);

int traducirRotulo(TRotulo rotulos[], int contRotulos, char rotulo[]);

int main(int argc, char *argv[])
{
    TRegistros registros = {0};
    TRam ram = {0};
    TRotulo rotulos[100];
    TInstruccionTexto instruccionesTexto[100];
    char* instrucciones[255];
    long codMaq;
    int contRotulos, contLinea, contInstrucciones, i = 0, huboError = 0, output = 1;
    char aux[4], linea[100];
    FILE* archEnt;
    FILE* archSalida;
    archEnt = fopen(argv[1],"rt");
    if (archEnt == NULL)
    {
        printf("No se encontro el archivo de entrada.\n");
        return 1;
    }
    generarTraduccion(instrucciones, &contInstrucciones);
    PrimeraPasada(archEnt,rotulos,&contRotulos,instruccionesTexto,&contLinea);
    registros[2] = contLinea * 3;
    registros[3] = 1000;
    if(argc>3 && strcmp("-o",argv[3])==0)
        output=0;
    while (i < contLinea)
    {
        strcpy(aux,"   ");
        codMaq = traducirMnemonico(instrucciones,contInstrucciones,instruccionesTexto[i].mnemonico);
        if (codMaq == -1)
        {
            huboError = 1;
            ram[i*3] = ram[i*3+1] = ram[i*3+2] = 0xFFFFFFFF;
            printf("No se encontro el mnemonico en la linea %d\n",i+1);
        }
        else
        {
            if (strcmp(instruccionesTexto[i].mnemonico,"STOP") == 0)
                ram[i*3] = codMaq << 16;
            else
                if (strcmp(instruccionesTexto[i].mnemonico,"SYS") == 0)
                {
                    ram[i*3] = codMaq << 16;
                    ram[i*3+1] = atoi(instruccionesTexto[i].arg1);
                }
                else
                {
                    ram[i*3] = (codMaq << 8);
                    argumentoGenerico(ram,i,instruccionesTexto[i].arg1,rotulos,contRotulos,&huboError,1);
                    if (strcmp(instruccionesTexto[i].mnemonico,"NOT") != 0 && (instruccionesTexto[i].mnemonico[0] != 'J' || (instruccionesTexto[i].mnemonico[0] == 'J' && (instruccionesTexto[i].mnemonico[1] == 'E' || instruccionesTexto[i].mnemonico[1] == 'G' || instruccionesTexto[i].mnemonico[1] == 'L'))))
                        argumentoGenerico(ram,i,instruccionesTexto[i].arg2,rotulos,contRotulos,&huboError,2);
                }
        }
        i++;
    }
    if (output)
    {
        printf("Direccion de memoria\t\t\tCodigo maquina\t\t\tLinea original\n");
        rewind(archEnt);
        for (i=0; i<contLinea; i++)
        {
            linea[0] = '\n';
            while (linea[0] == '\n' || linea[0] == '/')
                fgets(linea,100,archEnt);
            printf("\n[%08X]\t\t\t%08X %08X %08X\t%s",i*3,ram[i*3],ram[i*3+1],ram[i*3+2],linea);
        }
    }
    fclose(archEnt);
    if(huboError==0)
    {
        archSalida = fopen(argv[2],"wb");
        fwrite(registros,sizeof(long),16,archSalida);
        fwrite(ram,sizeof(long),2000,archSalida);
        fclose(archSalida);
    }
    system("pause");
    return 0;
}

void generarTraduccion(char* instrucciones[], int* contInstrucciones)
{
    int i;
    *contInstrucciones = 144;
    for (i=0; i<144; i++)
    {
        instrucciones[i] = (char*) malloc(5*sizeof(char));
        strcpy(instrucciones[i],"");
    }
    strcpy(instrucciones[0x01],"MOV");
    strcpy(instrucciones[0x02],"ADD");
    strcpy(instrucciones[0x03],"SUB");
    strcpy(instrucciones[0x04],"MUL");
    strcpy(instrucciones[0x05],"DIV");
    strcpy(instrucciones[0x06],"MOD");
    strcpy(instrucciones[0x13],"CMP");
    strcpy(instrucciones[0x17],"SWAP");
    strcpy(instrucciones[0x19],"RND");
    strcpy(instrucciones[0x31],"AND");
    strcpy(instrucciones[0x32],"OR");
    strcpy(instrucciones[0x33],"NOT");
    strcpy(instrucciones[0x34],"XOR");
    strcpy(instrucciones[0x37],"SHL");
    strcpy(instrucciones[0x38],"SHR");
    strcpy(instrucciones[0x20],"JMP");
    strcpy(instrucciones[0x21],"JE");
    strcpy(instrucciones[0x22],"JG");
    strcpy(instrucciones[0x23],"JL");
    strcpy(instrucciones[0x24],"JZ");
    strcpy(instrucciones[0x25],"JP");
    strcpy(instrucciones[0x26],"JN");
    strcpy(instrucciones[0x27],"JNZ");
    strcpy(instrucciones[0x28],"JNP");
    strcpy(instrucciones[0x29],"JNN");
    strcpy(instrucciones[0x81],"SYS");
    strcpy(instrucciones[0x8F],"STOP");
}

void PrimeraPasada(FILE* archEnt, TRotulo rotulos[], int* contRotulos,  TInstruccionTexto instrucciones[], int* contLinea)
{
    char linea[100], aux[100], *token;
    (*contLinea) = (*contRotulos) = 0;
    while (fgets(linea,100,archEnt) != NULL)
    {
        if (linea[0] != '\n' && linea[0] != '/')
        {
            (*contLinea)++;
            strcpy(aux,linea);
            token = strtok(linea," \t\n,");
            if (token[strlen(token)-1] == ':')
            {
                token[strlen(token)-1] = '\0';
                rotulos[*contRotulos].linea = (*contLinea);
                strcpy(rotulos[*contRotulos].rotulo,strupr(token));
                (*contRotulos)++;
                token = strtok(NULL,"\n\t ,/");
            }
            if (token != NULL)
            {

                strcpy(instrucciones[(*contLinea)-1].mnemonico,strupr(token));
                token = strtok(NULL,"\n\t ,/");
                if (token != NULL)
                {
                    if (token[0] == '\'')
                        strcpy(instrucciones[(*contLinea)-1].arg1,token);
                    else
                        strcpy(instrucciones[(*contLinea)-1].arg1,strupr(token));
                    token = strtok(NULL,"\n\t ,/");
                    if (token != NULL)
                        if (token[0] == '\'')
                            strcpy(instrucciones[(*contLinea)-1].arg2,token);
                        else
                            strcpy(instrucciones[(*contLinea)-1].arg2,strupr(token));
                }
            }
            //printf("\n|%s|%s|%s|\n",instrucciones[(*contLinea)-1].mnemonico,instrucciones[(*contLinea)-1].arg1,instrucciones[(*contLinea)-1].arg2); //Muestra los componentes de la instruccion de cada linea
        }
    }
}


void MostrarRotulos(int cant, TRotulo rotulos[])
{
    int i;
    for (i=0; i<cant; i++)
        printf("Linea: %d\tRotulo: %s\n", rotulos[i].linea,rotulos[i].rotulo);
}

void argumentoGenerico(TRam ram, int i, char argumento[], TRotulo rotulos[], int contRotulos, int* huboError, int numArg)
{
    char aux[4], car;
    int k = 0, j;
    if (argumento[0] == '[')
    {
        ram[i*3] += 2;
        if (argumento[1] == 'E')
        {
            j = 4;
            ram[i*3+numArg] = 3;
        }
        else
            if (argumento[1] == 'D')
            {
                j = 4;
                ram[i*3+numArg] = 2;
            }
            else
            {
                j = 1;
                ram[i*3+numArg] = 2;
            }
        while (j < strlen(argumento) && argumento[j] != ']')
        {
            aux[k] = argumento[j];
            j++;
            k++;
        }
        aux[k] = '\0';
        ram[i*3+numArg] = (ram[i*3+numArg] << 28) + atoi(aux);
    }
    else
    {
        if (argumento[0] >= 65 && strlen(argumento) > 2)
        {
            ram[i*3+numArg] = traducirRotulo(rotulos,contRotulos,argumento);
            if (ram[i*3+numArg] == 0xFFFFFFFF)
            {
                *huboError = 1;
                printf("No se encontro el rotulo %s\n\t\t\t\t",argumento);
            }

        }
        else
            if (argumento[1] == 'C')
            {
                ram[i*3] +=1;
                ram[i*3+numArg] = argumento[0] - 55;
            }
            else
                if (argumento[1] == 'X')
                {
                    ram[i*3] +=1;
                    ram[i*3+numArg] = argumento[0] - 55;
                }
                else
                    if (argumento[0] == 'A')
                    {
                        ram[i*3] += 1;
                        ram[i*3+numArg] = 8;
                    }
                    else
                        if (argumento[0] == 'C')
                        {
                            ram[i*3] += 1;
                            ram[i*3+numArg] = 9;
                        }
                        else
                        {
                            j = 1;
                            if  (argumento[0] >= '0' && argumento[0] <= '9')
                                ram[i*3+numArg] = atoi(argumento);
                            else
                                if (argumento[0] == '#')
                                {
                                    argumento[0] = '0';
                                    ram[i*3+numArg] = atoi(argumento);
                                }
                                else
                                    if (argumento[0] == '\'')
                                        ram[i*3+numArg] = argumento[1];
                                    else
                                        if (argumento[0] == '@')
                                            for(k=strlen(argumento)-2; k>=0; k--)
                                            {
                                                car = argumento[j++] - 48;
                                                ram[i*3+numArg] = (ram[i*3+numArg]<<3) + (car & 0x7);
                                            }
                                        else //Por descarte es %, hexadecimal
                                            for(k=strlen(argumento)-2; k>=0; k--)
                                            {
                                                car = argumento[j++];
                                                if (car <= '9')
                                                    car -= 48;
                                                else
                                                    car -= 55;
                                                ram[i*3+numArg] = (ram[i*3+numArg]<<4) + (car & 0xF);
                                            }
                        }
    }
    if (numArg == 1)
        ram[i*3] = ram[i*3] << 8;
}

long traducirMnemonico(char* instrucciones[], int contInstrucciones, char mnemonico[])
{
    int i = 0;
    while (i < contInstrucciones && strcmp(mnemonico,instrucciones[i]) != 0)
        i++;
    if (i < contInstrucciones)
        return i;
    else
        return -1;
}

int traducirRotulo(TRotulo rotulos[], int contRotulos, char rotulo[])
{
    int i = 0;
    while (i < contRotulos && strcmp(rotulo,rotulos[i].rotulo) != 0)
        i++;
    if (i < contRotulos)
        return rotulos[i].linea;
    else
        return -1;
}
