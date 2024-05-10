#include "componentes.h"
#include "const.h"
int extiendenegativo(int i,int cant){
    if(cant==1){
        return (i | 0xFFFFFF00);
    }
    else{
        return (i | 0xFFFF0000);
    }
}
void AnalizaSegmento(maquinaVirtual *MV,int posicion,int cant){
    int indice =MV->registro[DS]>>16;
    if((posicion < MV->segmento[indice]>>16) || (posicion + cant) >= 16384){
        printf("segmentation fall en %08X\n",MV->registro[IP] );
        exit(0);
    }
}
int obtienePunteroMemoria(maquinaVirtual *MV,int op){
    int reg = (op & 0x000F0000)>>16,offset1,offset2;
    if(reg==0){
        reg=DS;
    }
    int sec = MV->registro[reg]>>16;
    int indice = MV->segmento[sec]>>16;
    offset1=op & 0x0000FFFF;
    offset2=MV->registro[reg]&0x0000FFFF;

    return indice + offset1 + offset2;
}

int leememoria(maquinaVirtual *MV,int cant,int posicion){
    int aux=0;
    AnalizaSegmento(MV,posicion,cant);
    for(int i=0;i<cant;i++){
        aux = aux<<8;
        aux |= MV->memoria[posicion+i] & 0x000000FF;
    }
    return aux;
}
void escribememoria(maquinaVirtual *MV,int cant,int posicion,int dato){
    int aux;
    AnalizaSegmento(MV,posicion,cant);
    for(int i=0;i<cant;i++){
        aux = (dato >>(8*(cant-1) - i*8)) & 0x000000FF;
        MV->memoria[posicion++]=aux;
    }
}

int leeregistro(maquinaVirtual *MV,int quebyte,int Reg){
    int aux = MV->registro[Reg];
    if(quebyte==EX){
        return aux;
    }
    else if(quebyte == LOW){
        aux=aux & 0x000000FF;

        if((aux & 0x00000080)!=0){
            aux = extiendenegativo(aux,1);
        }
        return aux;
    } else if (quebyte == HIGH){
        aux=(aux & 0x0000FF00)>>8;
        if((aux & 0x00000080)!=0){
            aux = extiendenegativo(aux,1);
        }
        return aux;
    } else  {
        aux=aux&0x0000FFFF;
        if((aux & 0x00008000)!=0){
            aux = extiendenegativo(aux,2);
        }
        return aux;
    }
}
void escriberegistro(maquinaVirtual *MV,int Reg,int dato){
    int secReg,codReg;
    codReg=Reg & 0x0000000F;
    secReg= (Reg & 0x000000F0)>> 4;
    if(secReg==EX){
        MV->registro[codReg] = dato;
    }
    else if(secReg== LOW){
        MV->registro[codReg] =  (MV->registro[codReg] & 0xFFFFFF00) | (dato & 0x000000FF);
    } else if (secReg == HIGH){
        MV->registro[codReg] =  (MV->registro[codReg] & 0xFFFF00FF) | ((dato & 0x000000FF)<<8);
    } else{
        MV->registro[codReg] =  (MV->registro[codReg] & 0xFFFF0000) | (dato & 0x0000FFFF);
    }
}
int obtieneOP(maquinaVirtual *MV,int OP,int tipoOP){
    int secReg,codReg,cantMemoria;
    if(tipoOP==3){
        cantMemoria = (((OP&0xC0000000)>>22) & 0x00000003)+1;
        return leememoria(MV,cantMemoria,obtienePunteroMemoria(MV,OP));
    }
    else if(tipoOP==1){
        secReg=(OP&0x000000F0)>>4;
        codReg=(OP&0x0000000F);
        return leeregistro(MV,secReg,codReg);
    }
    else{
        if ( (OP&0x00008000) != 0 ){
            OP = extiendenegativo(OP,2);
        }
        return OP;
    }
}