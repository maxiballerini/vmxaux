#include "componentes.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CS 0
#define DS 1
#define ES 2
#define SS 3
#define KS 4
#define IP 5
#define CC 8
#define BP 7
#define CC 8
#define AC 9
#define EAX 10
#define EBX 11
#define ECX 12
#define EDX 13
#define EEX 14
#define EFX 15
void inicializaTablaSegmentosVersion1(maquinaVirtual *MV,int i,uint16_t tamanoCodigo){
    MV->segmento[0]=i-1;
    int aux = tamanoCodigo-i;
    MV->segmento[1] =0;
    MV->segmento[1] = MV->segmento[1]|  (aux& 0x0000FFFF);
    MV->segmento[1] = MV->segmento[1] | (i-1)<<16;

}
void inicializaTablaSegmentosVmx(maquinaVirtual *MV,uint16_t codeS,uint16_t dataS,uint16_t extraS,uint16_t stackS,uint16_t constS,int tamanoMemoria){
    int aux=0;
    MV->segmento[CS] = 0;
    MV->segmento[CS] += codeS;
    aux+=codeS;

    MV->segmento[DS] = (MV->segmento[DS] & aux ) < 16;
    MV->segmento[DS] += dataS; 
    aux+=dataS;

    MV->segmento[ES] = (MV->segmento[ES] & aux ) < 16;
    MV->segmento[ES] += dataS; 
    aux+=extraS;

    MV->segmento[SS] = (MV->segmento[SS] & aux ) < 16;
    MV->segmento[SS] += dataS; 
    aux+=stackS;

    MV->segmento[KS] = (MV->segmento[KS] & aux ) < 16;
    MV->segmento[KS] += dataS; 
    aux+=constS;

    if (aux>tamanoMemoria*1024){
        exit(0);
    }
}

void leeVersion1(maquinaVirtual *MV,FILE *arch){
    char aux;
    uint16_t tamanoCodigo=0;
    int i=0;
    while(!feof(arch)){
        fread(&aux,1,1,arch);
        MV->memoria[i]=aux;
        i++;
    }
    MV->registro[CS]=0x00000000;
    MV->registro[DS]=0x00010000;
    inicializaTablaSegmentosVersion1(MV,i,tamanoCodigo);
    MV->registro[IP]=0;
}
void leeVerision2VMX(maquinaVirtual *MV,FILE *arch,int tamanoMemoria){
    uint16_t codeS,dataS,extraS,stackS,constS;
    fread(&codeS, 1, 2, arch);
    fread(&dataS, 1, 2, arch);
    fread(&extraS, 1, 2, arch);
    fread(&stackS, 1, 2, arch);
    fread(&constS, 1, 2, arch);
    inicializaTablaSegmentosVmx(MV,codeS,dataS,extraS,stackS,constS,tamanoMemoria);


}
void leeVerision2VMI(maquinaVirtual *MV,FILE *arch,int tamanoMemoria){

}

void leeArch(maquinaVirtual *MV,char *nombreArchivoVMX,char *nombreArchivoVMI,int *aux,int *mostratAssembler,int tamanoMemoria){
    FILE *arch;
    char identificador[5];
    uint8_t version;
    if(!nombreArchivoVMX)
        arch = fopen(nombreArchivoVMI,"rb");
    else
        arch = fopen(nombreArchivoVMX,"rb");
    if(arch){
        *aux=1;
        fread(identificador, 1, 5, arch);
        fread(&version, 1, 1, arch);
        if(version==1){
            leeVersion1(MV,arch);
        }
        else{
            if(!nombreArchivoVMX)
                leeVerision2VMI(MV,arch,tamanoMemoria);
            else
                leeVerision2VMX(MV,arch,tamanoMemoria);
        }
        fclose(arch);
    }
}

