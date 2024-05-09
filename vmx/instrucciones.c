#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "componentes.h"
#define IP 5
#define DS 1
#define X 3
#define EX 0
#define HIGH 2
#define LOW 1
#define CC 8
#define AC 9
typedef void (*FuncPtr0)(maquinaVirtual *);
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
int obtieneOP(maquinaVirtual *MV,int OP,int tipoOP,int cantMemoria){
    int secReg,codReg;
    if(tipoOP==3){
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
//2 operandos
void MOV(maquinaVirtual*MV,int opA,int opB,char tipoA,char tipoB){
    int datoB = obtieneOP(MV,opB,tipoB,4);
    if(tipoA==3)
        escribememoria(MV,4,obtienePunteroMemoria(MV,opA),datoB);
    else
        escriberegistro(MV,opA,datoB);
}
void ADD(maquinaVirtual *MV, int opA, int opB, char tipoA, char tipoB) {
    int datoB = obtieneOP(MV,opB,tipoB,4);
    int datoA = obtieneOP(MV,opA,tipoA,4);
    datoA=datoA+datoB;
    if(datoA==0)
        MV->registro[CC]=0x00000001;
    else if(datoA<0){
        MV->registro[CC]=0x80000000;
    }
    else
        MV->registro[CC]=0;
    if(tipoA==3)
        escribememoria(MV,4,obtienePunteroMemoria(MV,opA),datoA);
    else
        escriberegistro(MV,opA,datoA);

}

void SUB(maquinaVirtual *MV, int opA, int opB, char tipoA, char tipoB) {
    int datoB = obtieneOP(MV,opB,tipoB,4);
    int datoA = obtieneOP(MV,opA,tipoA,4);
    datoA=datoA-datoB;
    if(datoA==0)
        MV->registro[CC]=0x00000001;
    else if(datoA<0){
        MV->registro[CC]=0x80000000;
    }
    else
        MV->registro[CC]=0;
    if(tipoA==3)
        escribememoria(MV,4,obtienePunteroMemoria(MV,opA),datoA);
    else
        escriberegistro(MV,opA,datoA);
}

void SWAP(maquinaVirtual *MV, int opA, int opB, char tipoA, char tipoB) {
    int datoB = obtieneOP(MV,opB,tipoB,4);
    int datoA = obtieneOP(MV,opA,tipoA,4);
    if(tipoA==3){
        escribememoria(MV,4,obtienePunteroMemoria(MV,opA),datoB);
        escribememoria(MV,4,obtienePunteroMemoria(MV,opB),datoA);
    }
    else{
        escriberegistro(MV,opA,datoB);
        escriberegistro(MV,opB,datoA);
    }
}

void MUL(maquinaVirtual *MV, int opA, int opB, char tipoA, char tipoB) {
    int datoB = obtieneOP(MV,opB,tipoB,4);
    int datoA = obtieneOP(MV,opA,tipoA,4);
    datoA=datoA*datoB;
    if(datoA==0)
        MV->registro[CC]=0x00000001;
    else if(datoA<0){
        MV->registro[CC]=0x80000000;
    }
    else
        MV->registro[CC]=0;
    if(tipoA==3)
        escribememoria(MV,4,obtienePunteroMemoria(MV,opA),datoA);
    else
        escriberegistro(MV,opA,datoA);
}

void DIV(maquinaVirtual *MV, int opA, int opB, char tipoA, char tipoB) {
    int datoB = obtieneOP(MV,opB,tipoB,4);
    int datoA = obtieneOP(MV,opA,tipoA,4);
    if (datoB == 0) {
        printf("Error: División por cero.\n");
        exit(0);
    }

    MV->registro[AC] = datoA % datoB;
    datoA = datoA / datoB;

    if(datoA==0)
        MV->registro[CC]=0x00000001;
    else if(datoA<0){
        MV->registro[CC]=0x80000000;
    }
    else
        MV->registro[CC]=0;

    if(tipoA==3)
        escribememoria(MV,4,obtienePunteroMemoria(MV,opA),datoA);
    else
        escriberegistro(MV,opA,datoA);
}

void CMP(maquinaVirtual *MV, int opA, int opB, char tipoA, char tipoB) {
    int datoB = obtieneOP(MV,opB,tipoB,4);
    int datoA = obtieneOP(MV,opA,tipoA,4);
    datoA=datoA-datoB;
    if(datoA==0)
        MV->registro[CC]=0x00000001;
    else if(datoA<0){
        MV->registro[CC]=0x80000000;
    }
    else
        MV->registro[CC]=0;
}

void SHL(maquinaVirtual *MV, int opA, int opB, char tipoA, char tipoB) {
    int datoB = obtieneOP(MV,opB,tipoB,4),secReg;
    int datoA = obtieneOP(MV,opA,tipoA,4);
    datoA=datoA<<datoB;
    if(tipoA==3)
        escribememoria(MV,4,obtienePunteroMemoria(MV,opA),datoA);
    else{
        secReg=(opA & 0x000000F0)>>4;
        if((secReg == LOW) || (secReg = HIGH)){
            datoA=datoA & 0x000000FF;
        }
        else if(secReg==X){
            datoA=datoA & 0x0000FFFF;
        }
        escriberegistro(MV,opA,datoA);
    }
}

void SHR(maquinaVirtual *MV, int opA, int opB, char tipoA, char tipoB) {
    int datoB = obtieneOP(MV,opB,tipoB,4),secReg;
    int datoA = obtieneOP(MV,opA,tipoA,4);
    datoA=datoA>>datoB;
    if((datoA & 0x80000000) == 0x80000000){
        for(int i=1;i<datoB;i++){
            datoA |= (1 << (31 - i));
        }
    }
    if(tipoA==3)
        escribememoria(MV,4,obtienePunteroMemoria(MV,opA),datoA);
    else{
        secReg=(opA & 0x000000F0)>>4;
        if((secReg == LOW) || (secReg = HIGH)){
            datoA=datoA & 0x000000FF;
        }
        else if(secReg==X){
            datoA=datoA & 0x0000FFFF;
        }
        escriberegistro(MV,opA,datoA);
    }
}

void AND(maquinaVirtual *MV, int opA, int opB, char tipoA, char tipoB) {
    int datoB = obtieneOP(MV,opB,tipoB,4);
    int datoA = obtieneOP(MV,opA,tipoA,4);
    datoA=datoA&datoB;
    if(datoA==0)
        MV->registro[CC]=0x00000001;
    else if(datoA<0){
        MV->registro[CC]=0x80000000;
    }
    else
        MV->registro[CC]=0;
    if(tipoA==3)
        escribememoria(MV,4,obtienePunteroMemoria(MV,opA),datoA);
    else
        escriberegistro(MV,opA,datoA);
}

void OR(maquinaVirtual *MV, int opA, int opB, char tipoA, char tipoB) {
       int datoB = obtieneOP(MV,opB,tipoB,4);
    int datoA = obtieneOP(MV,opA,tipoA,4);
    datoA=datoA|datoB;
    if(datoA==0)
        MV->registro[CC]=0x00000001;
    else if(datoA<0){
        MV->registro[CC]=0x80000000;
    }
    else
        MV->registro[CC]=0;
    if(tipoA==3)
        escribememoria(MV,4,obtienePunteroMemoria(MV,opA),datoA);
    else
        escriberegistro(MV,opA,datoA);
}

void XOR(maquinaVirtual *MV, int opA, int opB, char tipoA, char tipoB) {
       int datoB = obtieneOP(MV,opB,tipoB,4);
    int datoA = obtieneOP(MV,opA,tipoA,4);
    datoA=datoA^datoB;
    if(datoA==0)
        MV->registro[CC]=0x00000001;
    else if(datoA<0){
        MV->registro[CC]=0x80000000;
    }
    else
        MV->registro[CC]=0;
    if(tipoA==3)
        escribememoria(MV,4,obtienePunteroMemoria(MV,opA),datoA);
    else
        escriberegistro(MV,opA,datoA);
}

void RND(maquinaVirtual *MV, int opA, int opB, char tipoA, char tipoB) {
    int datoB = obtieneOP(MV,opB,tipoB,4);
    srand(time(NULL));
    datoB = rand() % (datoB+1);

    if(tipoA==3)
        escribememoria(MV,4,obtienePunteroMemoria(MV,opA),datoB);
    else
        escriberegistro(MV,opA,datoB);
}


//CORREGIR READ Y WRITE
void read(maquinaVirtual *MV){
    int formato=MV->registro[10]&0x000000FF;
    int CH=(MV->registro[12]&0x0000FF00)>>8;
    int CL=MV->registro[12]&0x000000FF,i;
    int aux;
    char c;
    int posicion =obtienePunteroMemoria(MV,13<<16);
    switch (formato) {
        case 4:
            for(i=0;i<CL;i++){
                printf("Introduce un carácter en formato octal: \n");
                scanf("%o", &aux);
                escribememoria(MV,CH,posicion+CH*i,aux);
            }
            break;
        case 8:
            for(i=0;i<CL;i++){
                printf("Introduce un carácter en formato hexadecimla: \n");
                scanf("%x", &aux);
                escribememoria(MV,CH,posicion+CH*i,aux);
            }
            break;
        case 1:
            for(i=0;i<CL;i++){
                printf("Introduce un carácter en formato decimal: \n");
                scanf("%d", &aux);
                escribememoria(MV,CH,posicion+CH*i,aux);
            }
            break;
        case 2:
            for(i=0;i<CL;i++){
                printf("Introduce un carácter: ");
                scanf("%c", &c);
                escribememoria(MV,CH,posicion+CH*i,aux);
            }
            break;
    }
}

void write(maquinaVirtual *MV){
    int formato=MV->registro[10]&0x000000FF;
    int CH=(MV->registro[12]&0x0000FF00)>>8;
    int CL=MV->registro[12]&0x000000FF,i;
    int aux,aux1;
    int posicion =obtienePunteroMemoria(MV,MV->registro[13]);
    for(i=0;i<CL;i++){
        aux = leememoria(MV,CH,posicion+(CH*i));
        printf("[%04X] ",posicion+(CH*i));

        if((formato & 0x00000008 )!=0 )
            printf("%% %X ",aux);

        if((formato & 0x00000004 )!=0 )
            printf("@ %o ",aux);

        if((formato & 0x00000002)!=0 ){
            printf("' ");
            aux=aux<<(24-(CH-1)*8);
            for(int j=0;j<CH;j++){
                aux1 =aux >>(24-8*j);
                if(aux1==127 ||(aux>=0 &&aux1<=31))
                    printf(".");
                else
                    printf("%c",aux1);
            }
            printf(" ");
        }

        if((formato & 0x00000001 )!=0 )
            printf("# %d",aux);

        printf("\n");
    }
}
//1 opereando
void SYS (maquinaVirtual *MV,int opA,char tipoA){
    FuncPtr0 funcionesSYS[2] = {read,write};
    int indice = obtieneOP(MV,opA,tipoA,4);
    funcionesSYS[indice - 1](MV);
}
void JMP (maquinaVirtual *MV,int opA,char tipoA){
    int dato = obtieneOP(MV,opA,tipoA,4);
    MV->registro[IP]=dato;
}
void JZ (maquinaVirtual *MV,int opA,int tipoA) {
    if(MV->registro[CC]  == 1 ){
        JMP(MV,opA,tipoA);
    }
}
void JP (maquinaVirtual *MV,int opA,int tipoA) {
    if(MV->registro[CC]  == 0 ){
        JMP(MV,opA,tipoA);
    }
}
void JN (maquinaVirtual *MV,int opA,int tipoA) {
    if(MV->registro[CC]  == -1 ){
        JMP(MV,opA,tipoA);
    }
}
void JNZ (maquinaVirtual *MV,int opA,int tipoA){
    if(MV->registro[CC]  == -1 || MV->registro[CC]  == 0){
        JMP(MV,opA,tipoA);
    }
}
void JNP (maquinaVirtual *MV,int opA,int tipoA){
    if(MV->registro[CC]  == 1 || MV->registro[CC]  == -1){
        JMP(MV,opA,tipoA);
    }
}
void JNN (maquinaVirtual *MV,int opA,int tipoA){
    if(MV->registro[CC]  == 1 || MV->registro[CC]  == 0){
        JMP(MV,opA,tipoA);
    }
}
void LDL (maquinaVirtual *MV,int opA,int tipoA){
    int dato=obtieneOP(MV,opA,tipoA,4);
    MV->registro[AC] = (MV->registro[AC] & 0xFFFF0000) | (dato & 0x0000FFFF);
}
void LDH (maquinaVirtual *MV,int opA,int tipoA){
    int dato=obtieneOP(MV,opA,tipoA,4);
    MV->registro[AC]=(MV->registro[AC] & 0x0000FFFF) | (dato & 0x0000FFFF)<<16;
}
void NOT (maquinaVirtual *MV,int opA,int tipoA){
    int datoA = obtieneOP(MV,opA,tipoA,4),secReg;
    datoA=~datoA;
    if(datoA==0)
        MV->registro[CC]=0x00000001;
    else if(datoA<0){
        MV->registro[CC]=0x80000000;
    }
    else
        MV->registro[CC]=0;

    if(tipoA==3)
        escribememoria(MV,4,obtienePunteroMemoria(MV,opA),datoA);
    else{
        secReg=(opA & 0x000000F0)>>4;
        if((secReg == LOW) || (secReg = HIGH)){
            datoA=datoA & 0x000000FF;
        }
        else if(secReg==X){
            datoA=datoA & 0x0000FFFF;
        }
        escriberegistro(MV,opA,datoA);
    }
}

//0 operandos
void STOP (maquinaVirtual *MV){
    exit(0);
}
