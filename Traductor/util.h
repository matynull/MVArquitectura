#define CS 0
#define DS 500
#define ES 1000

typedef long TRam[2000];

typedef long TRegistros[16];

typedef struct
{
    int codigoMaquina;
    char nmonico[5];
} TInstruccion;

typedef struct
{
    int linea;
    char rotulo[10];
} TRotulo;
