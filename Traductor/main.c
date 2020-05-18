#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "util.h"

typedef struct
{
    char mnemonico[11], arg1[20], arg2[20];
} TInstruccionTexto;

typedef struct
{
    int valor, directa;
    char constante[11], string[18];
} TConstante;

void generarTraduccion(char* mnemonicos[], int* contMnemonicos, char* indiceRegistros[]);

void primeraPasada(FILE* archEnt, TRotulo rotulos[], int* contRotulos, TConstante constantes[], int* contConstantes, TInstruccionTexto instrucciones[], int* contLinea, TRegistros registros, char* indiceRegistros[], int* huboError, TRam ram);

void mostrarRotulos(TRotulo rotulos[], int cant);

long traducirMnemonico(char* mnemonicos[], int contMnemonicos, char arg[]);

void argumentoGenerico(TRam ram, int i, char argumento[], TRotulo rotulos[], int contRotulos, TConstante constantes[], int contConstantes, char* indiceRegistros[], int* huboError, int numArg);

int traducirRotulo(TRotulo rotulos[], int contRotulos, char rotulo[]);

int traducirRegistro(char* indiceRegistros[], char arg[]);

int traducirConstante(TConstante constantes[], int contConstantes, char arg[]);

long operandoDirectoOIndirecto(TConstante constantes[], int contConstantes, char* indiceRegistros[], char arg[], int indirecto);

long operandoInmediato(char argumento[]);

int main(int argc, char *argv[])
{
    TRegistros registros = {0};
    TRam ram = {0};
    TRotulo rotulos[100];
    TInstruccionTexto instruccionesTexto[100];
    TConstante constantes[30];
    char* mnemonicos[255];
    char* indiceRegistros[16];
    long codMaq;
    int contRotulos, contLinea, contMnemonicos, contConstantes, i = 0, huboError = 0, output = 1;
    char linea[100], aux[100], *aux2, aux3[100];
    FILE* archEnt;
    FILE* archSalida;
    archEnt = fopen(argv[1],"rt");
    if (archEnt == NULL)
    {
        printf("ERROR: No se encontro el archivo de entrada.\n");
        return 1;
    }
    generarTraduccion(mnemonicos, &contMnemonicos, indiceRegistros);
    primeraPasada(archEnt,rotulos,&contRotulos,constantes,&contConstantes,instruccionesTexto,&contLinea,registros,indiceRegistros,&huboError,ram);
    //mostrarRotulos(rotulos,contRotulos); //COMENTAR ESTO
    if(argc>3 && strcmp("-o",argv[3])==0)
        output=0;
    while (i < contLinea)
    {
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
                    if (codMaq == 0x48)
                    {
                        ram[i*3] = codMaq << 16;
                    }
                    else
                    {
                        ram[i*3] = (codMaq << 8);
                        argumentoGenerico(ram,i,instruccionesTexto[i].arg1,rotulos,contRotulos,constantes,contConstantes,indiceRegistros,&huboError,1);
                        if (codMaq != 0x20 && codMaq != 0x33 && (codMaq < 0x24 || codMaq > 0x29) && codMaq != 0x40 && codMaq != 0x44 && codMaq != 0x45)
                            argumentoGenerico(ram,i,instruccionesTexto[i].arg2,rotulos,contRotulos,constantes,contConstantes,indiceRegistros,&huboError,2);
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
            {
                fgets(linea,100,archEnt);
                strcpy(aux,linea);
                aux2 = strtok(aux," \t\n");
                strcpy(aux3,linea);
            }
            while (aux2 == NULL || aux2[0] == '\\' || strstr(strupr(aux3),"EQU") || aux2[0] == '/');
            printf("\n[%08X]\t\t\t%08X %08X %08X\t%s",i*3,ram[i*3],ram[i*3+1],ram[i*3+2],linea);
        }
    }
    fclose(archEnt);
    if(huboError==0)
    {
        archSalida = fopen(argv[2],"wb");
        fwrite(registros,sizeof(long),16,archSalida);
        fwrite(ram,sizeof(long),registros[2],archSalida);
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
    strcpy(mnemonicos[0x31],"AND");
    strcpy(mnemonicos[0x32],"OR");
    strcpy(mnemonicos[0x33],"NOT");
    strcpy(mnemonicos[0x34],"XOR");
    strcpy(mnemonicos[0x37],"SHL");
    strcpy(mnemonicos[0x38],"SHR");
    strcpy(mnemonicos[0x40],"CALL");
    strcpy(mnemonicos[0x44],"PUSH");
    strcpy(mnemonicos[0x45],"POP");
    strcpy(mnemonicos[0x48],"RET");
    strcpy(mnemonicos[0x50],"SLEN");
    strcpy(mnemonicos[0x51],"SMOV");
    strcpy(mnemonicos[0x53],"SCMP");
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

void primeraPasada(FILE* archEnt, TRotulo rotulos[], int* contRotulos, TConstante constantes[], int* contConstantes, TInstruccionTexto instrucciones[], int* contLinea, TRegistros registros, char* indiceRegistros[], int* huboError, TRam ram)
{
    int DATA=500,EXTRA=500,STACK=500,i,j,k,tamanoConstantes = 0, esString = 0;
    char linea[100], aux[100], aux2[100], aux3[100], auxReg, *token;
    (*contLinea) = (*contRotulos) = (*contConstantes) = 0;
    while (fgets(linea,100,archEnt) != NULL)
    {
        strcpy(aux,linea);
        token = strtok(aux," \t\n");
        if (token != NULL && token[0] != '/')
        {
            if (linea[0] == '\\') //La linea es una directiva
            {
                strcpy(aux,linea);
                strupr(aux);
                token = strtok(aux," \t\n\\");
                while(token != NULL && token[0] != '/')
                {
                    token = strtok(NULL," \t=");
                    if (token != NULL)
                    {
                        auxReg = token[0];
                        token = strtok(NULL," \t=");
                        if (token != NULL)
                        {
                            switch (auxReg)
                            {
                                case 'D':
                                    DATA = atoi(token);
                                    break;
                                case 'E':
                                    EXTRA = atoi(token);
                                    break;
                                case 'S':
                                    STACK = atoi(token);
                                    break;
                            }
                        }
                    }
                }
            }
            else //La linea es una instruccion o una constante
            {
                (*contLinea)++;
                strcpy(aux,linea);
                token = strtok(aux," \t\n,");
                if (token[strlen(token)-1] == ':') //Hay rotulo
                {
                    token[strlen(token)-1] = '\0';
                    if (traducirRotulo(rotulos,*contRotulos,strupr(token)) != -1)
                    {
                        *huboError = 1;
                        printf("ERROR: Ya existe el rotulo %s\n\t\t\t\t",strupr(token));
                    }
                    else //No existia como rotulo
                    {
                        if (traducirConstante(constantes,*contConstantes,strupr(token)) != -32000)
                        {
                            *huboError = 1;
                            printf("ERROR: Ya existe la constante %s\n\t\t\t\t",strupr(token));
                        }
                        else //No existia como rotulo ni como constante
                        {
                            rotulos[*contRotulos].linea = (*contLinea);
                            strcpy(rotulos[*contRotulos].rotulo,strupr(token));
                            (*contRotulos)++;
                        }
                    }
                    token = strtok(NULL," \t\n,"); //Se lee el mnemonico
                }
                if (token != NULL && token[0] != '/')
                {
                    strcpy(instrucciones[(*contLinea)-1].mnemonico,strupr(token));
                    token = strtok(NULL," \t\n,"); //Se lee el argumento 1
                    if (token != NULL && token[0] != '/')
                    {
                        if (token[0] == '\'')
                            strcpy(instrucciones[(*contLinea)-1].arg1,token);
                        else
                            strcpy(instrucciones[(*contLinea)-1].arg1,strupr(token));
                        token = strtok(NULL,"\n"); //Se empieza a leer el argumento 2
                        if (token != NULL && token[0] != '/')
                        {
                            strcpy(aux2,token);
                            strcpy(aux3,aux2);
                            token = strtok(aux2,"\""); //Se buscan los caracteres entre el principio de aux2 y una comilla. Si el primer caracter de aux2 es una comilla, token es el String
                            if (token != NULL && strcmp(token,aux3) != 0) //El argumento 2 es un String
                            {
                                esString = 1;
                                if (aux3[0] != '"') //Token no es el string. Se busca la siguiente comilla
                                    token = strtok(NULL,"\"");
                                strcpy(instrucciones[(*contLinea)-1].arg2,token);
                            }
                            else //El argumento 2 no es un String
                            {
                                token = strtok(aux3," \t\n,/");
                                if (token != NULL && token[0] != '/')
                                    if (token[0] == '\'') //Si el argumento es un caracter ASCII es case sensitive
                                        strcpy(instrucciones[(*contLinea)-1].arg2,token);
                                    else
                                        strcpy(instrucciones[(*contLinea)-1].arg2,strupr(token));
                            }
                        }
                    }
                }
                if (strcmp(instrucciones[(*contLinea)-1].arg1,"EQU") == 0) //La linea declara una constante
                {
                    if (traducirRotulo(rotulos,*contRotulos,instrucciones[(*contLinea)-1].mnemonico) != -1)
                    {
                        *huboError = 1;
                        printf("ERROR: Ya existe el rotulo %s\n\t\t\t\t",instrucciones[(*contLinea)-1].mnemonico);
                    }
                    else //No existia como rotulo
                    {
                        if (traducirConstante(constantes,*contConstantes,instrucciones[(*contLinea)-1].mnemonico) != -32000)
                        {
                            *huboError = 1;
                            printf("ERROR: Ya existe la constante %s\n\t\t\t\t",instrucciones[(*contLinea)-1].mnemonico);
                        }
                        else //No existia como rotulo ni como constante
                        {
                            if (esString) //La constante es un String
                            {
                                constantes[*contConstantes].directa = 1;
                                strcpy(constantes[*contConstantes].string,instrucciones[(*contLinea)-1].arg2);
                                constantes[*contConstantes].valor = 0;
                            }
                            else //La constante es inmediata
                            {
                                constantes[*contConstantes].directa = 0;
                                constantes[*contConstantes].valor = operandoInmediato(instrucciones[(*contLinea)-1].arg2);
                            }
                            strcpy(constantes[*contConstantes].constante,instrucciones[(*contLinea)-1].mnemonico);
                            (*contConstantes)++;
                            (*contLinea)--;
                        }
                    }
                }
                //else
                    //COMENTAR ESTO
                    //printf("\n|%s|%s|%s|\n",instrucciones[(*contLinea)-1].mnemonico,instrucciones[(*contLinea)-1].arg1,instrucciones[(*contLinea)-1].arg2); //Muestra los componentes de la instruccion de cada linea
            }
        }
    }
    k = (*contLinea) * 3;
    for (i=0; i<(*contConstantes); i++)
        if (constantes[i].directa)
        {
            constantes[i].valor = k;
            for (j=0; j<=strlen(constantes[i].string); j++)
            {
                tamanoConstantes++;
                ram[k] = constantes[i].string[j];
                k++;
            }
        }
    registros[2] = (*contLinea) * 3 + tamanoConstantes;
    if (EXTRA != -1)
    {
        registros[3] = registros[2] + DATA;
        registros[5] = registros[3] + EXTRA;
    }
    else
    {
        registros[3] = -1;
        registros[5] = registros[2] + DATA;
    }
    registros[6] = STACK;
    registros[0] = registros[5] + registros[6];
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

void argumentoGenerico(TRam ram, int i, char argumento[], TRotulo rotulos[], int contRotulos, TConstante constantes[], int contConstantes, char* indiceRegistros[], int* huboError, int numArg)
{
    int esDirecto;
    char* token;
    char aux[20], aux2[20];
    if (argumento[0] == '[')
    {
        strcpy(aux,argumento);
        esDirecto = (argumento[1] >= 48 && argumento[1] <= 57);
        esDirecto |= (argumento[3] == ':' && argumento[4] >= 48 && argumento[4] <= 57);
        if (!esDirecto)
        {
            token = strtok(aux,"[]:+-");
            strcpy(aux2,token);
            token = strtok(NULL,"[]:+-");
            if (token == NULL)
            {
                if (traducirRegistro(indiceRegistros,aux2) == -1)
                    esDirecto = 1;
            }
            else
                if (argumento[3] != '+' && argumento[3] != '-' && traducirRegistro(indiceRegistros,token) == -1)
                    esDirecto = 1;
        }
        if (esDirecto)
        {
            ram[i*3] += 2;
            ram[i*3+numArg] = operandoDirectoOIndirecto(constantes,contConstantes,indiceRegistros,argumento,0);
        }
        else
        {
            ram[i*3] += 3;
            ram[i*3+numArg] = operandoDirectoOIndirecto(constantes,contConstantes,indiceRegistros,argumento,1);
        }
        if (ram[i*3+numArg] == -1)
        {
            *huboError = 1;
            printf("ERROR: Hay un error en el operando.\n\t\t\t\t");
        }
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
            else //Es rotulo o constante
            {
                ram[i*3+numArg] = traducirRotulo(rotulos,contRotulos,argumento);
                if (ram[i*3+numArg] == 0xFFFFFFFF)
                {
                    ram[i*3+numArg] = traducirConstante(constantes,contConstantes,argumento);
                    if (ram[i*3+numArg] == -32000)
                    {
                        *huboError = 1;
                        ram[i*3+numArg] = 0xFFFFFFFF;
                        printf("ERROR: No se encontro el simbolo %s\n\t\t\t\t",argumento);
                    }
                }
            }
        }
        else //Es inmediato, no rotulo ni constante
            ram[i*3+numArg] = operandoInmediato(argumento);
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

int traducirConstante(TConstante constantes[], int contConstantes, char arg[])
{
    int i = 0;
    while (i < contConstantes && strcmp(arg,constantes[i].constante) != 0)
        i++;
    if (i < contConstantes)
        return constantes[i].valor;
    else
        return -32000;
}

long operandoDirectoOIndirecto(TConstante constantes[], int contConstantes, char* indiceRegistros[], char arg[], int indirecto)
{
    int constante;
    long retorno = 0;
    char* token;
    char aux[20],aux2[20],aux3[20];
    int hayReg = 0, reg1, reg2 = 0, reg3, signo = 1;
    strcpy(aux,arg);
    strcpy(aux2,aux);
    token = strtok(aux,"[:"); //Busca hasta el primer :
    if (token[strlen(token)-1] != ']') //Se especifica registro base
    {
        hayReg = 1;
        reg1 = traducirRegistro(indiceRegistros,token);
        if (strlen(aux2) >= 7 && aux2[6] == '-')
            signo = -1;
        token = strtok(NULL,"+-");
    }
    else //No se especifica registro base
    {
        if (strlen(token) >= 3 && token[2] == '-')
            signo = -1;
        strcpy(aux3,token);
        token = strtok(aux3,"+-");
    }
    if (token[strlen(token)-1] != ']') //Hay suma o resta
    {
        reg2 = traducirRegistro(indiceRegistros,token);
        if (reg2 == -1 || (reg2 >= 1 && reg2 <= 5) || reg2 == 9)
            retorno = -1;
        else
            retorno = reg2;
        token = strtok(NULL,"]");
    }
    else //No hay suma ni resta
    {
        strcpy(aux3,token);
        token = strtok(aux3,"]");
    }
    if (retorno != -1)
        if (token[0] >= 48 && token[0] <= 57) //El primer caracter del token es un numero
            if (indirecto)
                retorno += ((signo * atoi(token)) & 0x00FFFFFF) << 4;
            else
                retorno += ((signo * atoi(token)) & 0x0FFFFFFF);
        else //El primer caracter del token no es un numero
        {
            reg3 = traducirRegistro(indiceRegistros,token);
            if (reg3 != -1) //El token era un registro
            {
                reg2 = reg3;
                retorno += reg2;
            }
            else //El token era una constante
            {
                constante = traducirConstante(constantes,contConstantes,token);
                if (constante == -32000)
                    retorno = -1;
                else
                    if (indirecto)
                        retorno += ((signo * constante) & 0x00FFFFFF) << 4;
                    else
                        retorno += ((signo * constante) & 0x0FFFFFFF);
            }
        }
    if (!hayReg) //Si no hay registro base busca el default en base al registro 2
        if (reg2 == 6 || reg2 == 7)
            reg1 = 5;
        else
            reg1 = 2;
    if ((reg1 >= 1 && reg1 <= 3) || (reg1 == 5))
        retorno += reg1 << 28;
    else
        retorno = -1;
    return retorno;
}

long operandoInmediato(char argumento[])
{
    char car;
    int k;
    long retorno=0;
    if  (argumento[0] >= '0' && argumento[0] <= '9' || argumento[0]=='-') //Es decimal
        retorno = atoi(argumento);
    else
        if (argumento[0] == '#') //Es decimal
        {
            argumento[0] = '0';
            retorno = atoi(argumento);
        }
        else
            if (argumento[0] == '\'') //Es caracter
                if (strlen(argumento) > 1)
                    retorno = argumento[1];
                else
                    retorno = 0x20;
            else
                if (argumento[0] == '@') //Es octal
                    for(k=1; strlen(argumento)>k; k++)
                    {
                        car = argumento[k] - 48;
                        retorno = (retorno<<3) + (car & 0x7);
                    }
                else //Es hexadecimal
                    for(k=1; strlen(argumento)>k; k++)
                    {
                        car = argumento[k];
                        if (car <= '9')
                            car -= 48;
                        else
                            car -= 55;
                        retorno = (retorno<<4) + (car & 0xF);
                    }
    return retorno;
}
