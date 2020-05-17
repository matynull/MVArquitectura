typedef long TRam[8192];

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
