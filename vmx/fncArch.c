#include "componentes.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "const.h"
void inicializaTablaSegmentosVersion1(maquinaVirtual *MV,int i,uint16_t tamanoCodigo){
    MV->segmento[0]=i-1;
    int aux = tamanoCodigo-i;
    MV->segmento[1] =0;
    MV->segmento[1] = MV->segmento[1]|  (aux& 0x0000FFFF);
    MV->segmento[1] = MV->segmento[1] | (i-1)<<16;

}
void inicializaTablaSegmentosVmx(maquinaVirtual *MV,uint16_t codeS,uint16_t dataS,uint16_t extraS,uint16_t stackS,uint16_t constS,int tamanoMemoria){
    int aux=0;
    if(KS>0){
        MV->segmento[KS] = (MV->segmento[CS] & aux ) < 16;
        MV->segmento[KS] += dataS; 
        aux+=constS;
    }
    if(CS>0){
        MV->segmento[CS] = (MV->segmento[CS] & aux ) < 16;
        MV->segmento[CS] += codeS;
        aux+=codeS;
    }
    if(DS>0){
        MV->segmento[DS] = (MV->segmento[DS] & aux ) < 16;
        MV->segmento[DS] += dataS; 
        aux+=dataS;
    }
    if(ES>0){
        MV->segmento[ES] = (MV->segmento[ES] & aux ) < 16;
        MV->segmento[ES] += dataS; 
        aux+=extraS;
    }
    if(SS>0){
        MV->segmento[SS] = (MV->segmento[SS] & aux ) < 16;
        MV->segmento[SS] += dataS; 
        aux+=stackS;
    }

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
    int flag;
    char aux;
    fread(&codeS, 1, 2, arch);
    fread(&dataS, 1, 2, arch);
    fread(&extraS, 1, 2, arch);
    fread(&stackS, 1, 2, arch);
    fread(&constS, 1, 2, arch);
    inicializaTablaSegmentosVmx(MV,codeS,dataS,extraS,stackS,constS,tamanoMemoria);
    flag=constS + codeS;
    for(int i=0;i<flag;i++){
        fread(&MV->memoria[i],1,1,arch);
    }

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

