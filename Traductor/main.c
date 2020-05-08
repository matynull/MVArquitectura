#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "util.h"

typedef struct
{
    char mnemonico[5], arg1[10], arg2[10];
} TInstruccionTexto;

void generarTraduccion(char* mnemonicos[], int* contMnemonicos, char* indiceRegistros[]);

void primeraPasada(FILE* archEnt, TRotulo rotulos[], int* contRotulos, TInstruccionTexto instrucciones[], int* contLinea);

void mostrarRotulos(TRotulo rotulos[], int cant);

long traducirMnemonico(char* mnemonicos[], int contMnemonicos, char arg[]);

void argumentoGenerico(TRam ram, int i, char argumento[], TRotulo rotulos[], int contRotulos, char* indiceRegistros[], int* huboError, int numArg);

int traducirRotulo(TRotulo rotulos[], int contRotulos, char rotulo[]);

int traducirRegistro(char* indiceRegistros[], char arg[]);

int main(int argc, char *argv[])
{
    TRegistros registros = {0};
    TRam ram = {0};
    TRotulo rotulos[100];
    TInstruccionTexto instruccionesTexto[100];
    char* mnemonicos[255];
    char* indiceRegistros[16];
    long codMaq;
    int contRotulos, contLinea, contMnemonicos, i = 0, huboError = 0, output = 1;
    char aux[4], linea[100];
    FILE* archEnt;
    FILE* archSalida;
    archEnt = fopen(argv[1],"rt");
    if (archEnt == NULL)
    {
        printf("ERROR: No se encontro el archivo de entrada.\n");
        return 1;
    }
    generarTraduccion(mnemonicos, &contMnemonicos, indiceRegistros);
    primeraPasada(archEnt,rotulos,&contRotulos,instruccionesTexto,&contLinea);
    //mostrarRotulos(rotulos,contRotulos); //COMENTAR ESTO
    registros[2] = contLinea * 3;
    registros[3] = 1000;
    if(argc>3 && strcmp("-o",argv[3])==0)
        output=0;
    while (i < contLinea)
    {
        strcpy(aux,"   ");
        codMaq = traducirMnemonico(mnemonicos,contMnemonicos,instruccionesTexto[i].mnemonico);
        if (codMaq == -1)
        {
            huboError = 1;
            ram[i*3] = ram[i*3+1] = ram[i*3+2] = 0xFFFFFFFF;
            printf("ERROR: No se encontro el mnemonico en la linea %d\n",i+1);
        }
        else
        {
            if (codMaq == 0x8f)
                ram[i*3] = codMaq << 16;
            else
                if (codMaq == 0x81)
                {
                    ram[i*3] = codMaq << 16;
                    ram[i*3+1] = atoi(instruccionesTexto[i].arg1);
                }
                else
                {
                    ram[i*3] = (codMaq << 8);
                    argumentoGenerico(ram,i,instruccionesTexto[i].arg1,rotulos,contRotulos,indiceRegistros,&huboError,1);
                    if (codMaq != 0x33 && codMaq != 0x20 && (codMaq < 24 || codMaq > 29))
                        argumentoGenerico(ram,i,instruccionesTexto[i].arg2,rotulos,contRotulos,indiceRegistros,&huboError,2);
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
            do
                fgets(linea,100,archEnt);
            while (linea[0] == '\n' || linea[0] == '/');
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

void generarTraduccion(char* mnemonicos[], int* contMnemonicos, char* indiceRegistros[])
{
    int i;

    *contMnemonicos = 144;
    for (i=0; i<144; i++)
    {
        mnemonicos[i] = (char*) malloc(5*sizeof(char));
        strcpy(mnemonicos[i],"");
    }
    strcpy(mnemonicos[0x01],"MOV");
    strcpy(mnemonicos[0x02],"ADD");
    strcpy(mnemonicos[0x03],"SUB");
    strcpy(mnemonicos[0x04],"MUL");
    strcpy(mnemonicos[0x05],"DIV");
    strcpy(mnemonicos[0x06],"MOD");
    strcpy(mnemonicos[0x13],"CMP");
    strcpy(mnemonicos[0x17],"SWAP");
    strcpy(mnemonicos[0x19],"RND");
    strcpy(mnemonicos[0x31],"AND");
    strcpy(mnemonicos[0x32],"OR");
    strcpy(mnemonicos[0x33],"NOT");
    strcpy(mnemonicos[0x34],"XOR");
    strcpy(mnemonicos[0x37],"SHL");
    strcpy(mnemonicos[0x38],"SHR");
    strcpy(mnemonicos[0x20],"JMP");
    strcpy(mnemonicos[0x21],"JE");
    strcpy(mnemonicos[0x22],"JG");
    strcpy(mnemonicos[0x23],"JL");
    strcpy(mnemonicos[0x24],"JZ");
    strcpy(mnemonicos[0x25],"JP");
    strcpy(mnemonicos[0x26],"JN");
    strcpy(mnemonicos[0x27],"JNZ");
    strcpy(mnemonicos[0x28],"JNP");
    strcpy(mnemonicos[0x29],"JNN");
    strcpy(mnemonicos[0x81],"SYS");
    strcpy(mnemonicos[0x8F],"STOP");

    for (i=0; i<16; i++)
        indiceRegistros[i] = (char*) malloc(5*sizeof(char));
    strcpy(indiceRegistros[0],"PS");
    strcpy(indiceRegistros[1],"CS");
    strcpy(indiceRegistros[2],"DS");
    strcpy(indiceRegistros[3],"ES");
    strcpy(indiceRegistros[4],"IP");
    strcpy(indiceRegistros[5],"SS");
    strcpy(indiceRegistros[6],"SP");
    strcpy(indiceRegistros[7],"BP");
    strcpy(indiceRegistros[8],"AC");
    strcpy(indiceRegistros[9],"CC");
    strcpy(indiceRegistros[10],"AX");
    strcpy(indiceRegistros[11],"BX");
    strcpy(indiceRegistros[12],"CX");
    strcpy(indiceRegistros[13],"DX");
    strcpy(indiceRegistros[14],"EX");
    strcpy(indiceRegistros[15],"FX");
}

void primeraPasada(FILE* archEnt, TRotulo rotulos[], int* contRotulos,  TInstruccionTexto instrucciones[], int* contLinea)
{
    char linea[100], aux[100], *token;
    (*contLinea) = (*contRotulos) = 0;
    while (fgets(linea,100,archEnt) != NULL)
    {
        if (linea[0] != '\n' && linea[0] != '/')
        {
            (*contLinea)++;
            strcpy(aux,linea);
            token = strtok(linea," \t\n,/");
            if (token[strlen(token)-1] == ':')
            {
                token[strlen(token)-1] = '\0';
                rotulos[*contRotulos].linea = (*contLinea);
                strcpy(rotulos[*contRotulos].rotulo,strupr(token));
                (*contRotulos)++;
                token = strtok(NULL," \t\n,/");
            }
            if (token != NULL)
            {
                strcpy(instrucciones[(*contLinea)-1].mnemonico,strupr(token));
                token = strtok(NULL," \t\n,/");
                if (token != NULL)
                {
                    if (token[0] == '\'')
                        strcpy(instrucciones[(*contLinea)-1].arg1,token);
                    else
                        strcpy(instrucciones[(*contLinea)-1].arg1,strupr(token));
                    token = strtok(NULL," \t\n,/");
                    if (token != NULL)
                        if (token[0] == '\'')
                            strcpy(instrucciones[(*contLinea)-1].arg2,token);
                        else
                            strcpy(instrucciones[(*contLinea)-1].arg2,strupr(token));
                }
            }
            //COMENTAR ESTO
            //printf("\n|%s|%s|%s|\n",instrucciones[(*contLinea)-1].mnemonico,instrucciones[(*contLinea)-1].arg1,instrucciones[(*contLinea)-1].arg2); //Muestra los componentes de la instruccion de cada linea
        }
    }
}


void mostrarRotulos(TRotulo rotulos[], int cant)
{
    int i;
    for (i=0; i<cant; i++)
        printf("Linea: %d\tRotulo: %s\n", rotulos[i].linea,rotulos[i].rotulo);
}

long traducirMnemonico(char* mnemonicos[], int contMnemonicos, char arg[])
{
    int i = 0;
    while (i < contMnemonicos && strcmp(arg,mnemonicos[i]) != 0)
        i++;
    if (i < contMnemonicos)
        return i;
    else
        return -1;
}

void argumentoGenerico(TRam ram, int i, char argumento[], TRotulo rotulos[], int contRotulos, char* indiceRegistros[], int* huboError, int numArg)
{
    char aux[4], car;
    int k = 0, j;
    if (argumento[0] == '[') //Es direccion de memoria
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
        if (argumento[0] >= 65 && argumento[0] <= 90)
        {
            if (strlen(argumento) == 2) //Es registro
            {
                ram[i*3] += 1;
                ram[i*3+numArg] = traducirRegistro(indiceRegistros,argumento);
                if (ram[i*3+numArg] == 0xFFFFFFFF)
                {
                    *huboError = 1;
                    printf("ERROR: No se encontro el registro %s\n\t\t\t\t",argumento);
                }
            }
            else //Es directo, rotulo
            {
                ram[i*3+numArg] = traducirRotulo(rotulos,contRotulos,argumento);
                if (ram[i*3+numArg] == 0xFFFFFFFF)
                {
                    *huboError = 1;
                    printf("ERROR: No se encontro el rotulo %s\n\t\t\t\t",argumento);
                }
            }
        }
        else //Es directo, no rotulo
        {
            j = 1;
            if  (argumento[0] >= '0' && argumento[0] <= '9') //Es decimal
               ram[i*3+numArg] = atoi(argumento);
            else
                if (argumento[0] == '#') //Es decimal
                {
                    argumento[0] = '0';
                    ram[i*3+numArg] = atoi(argumento);
                }
                else
                    if (argumento[0] == '\'') //Es caracter
                        ram[i*3+numArg] = argumento[1];
                    else
                        if (argumento[0] == '@') //Es octal
                            for(k=strlen(argumento)-2; k>=0; k--)
                            {
                                car = argumento[j++] - 48;
                                ram[i*3+numArg] = (ram[i*3+numArg]<<3) + (car & 0x7);
                            }
                        else //Es hexadecimal
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

int traducirRegistro(char* indiceRegistros[], char arg[])
{
    int i = 0;
    while (i < 16 && strcmp(arg,indiceRegistros[i]) != 0)
        i++;
    if (i < 16)
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
