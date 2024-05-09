#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "disassembler.h"
#include "componentes.h"
#include "fncArch.h"
#include "const.h"
#include "ejecucion.h"

int main(int argc,char *argv[])
{
    maquinaVirtual MV;
    char *nombreArchivoVMX = NULL, *nombreArchivoVMI = NULL;
    int tamanoMemoria = 16, mostrarAssembler = 0,aux=0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-d") == 0) {
            mostrarAssembler = 1;
        } else if (strncmp(argv[i], "m=", 2) == 0) {
            tamanoMemoria = atoi(argv[i] + 2);
        }else if (strstr(argv[i], ".vmx") != NULL) {
            nombreArchivoVMX = argv[i];
        }
        else {
            nombreArchivoVMI = argv[i];
        }
    }
    
    leeArch(&MV,nombreArchivoVMX,nombreArchivoVMI,&aux,&mostrarAssembler,tamanoMemoria);
    if(aux){
        if(mostrarAssembler){
            printf("ejecutando dissasembler\n");
            ejecutadisasssembler(MV);
        }
        printf("\n---------------------------------------------------------------------\n");
        ejecutaInstrucciones(MV);
    }
    else
        printf("el archivo no se pudo abrir");
    return 0;
}