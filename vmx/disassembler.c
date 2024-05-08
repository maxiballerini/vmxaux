#include "componentes.h"
#include <stdio.h>
#include <stdlib.h>
void ejecutadisasssembler(maquinaVirtual MV){
    int opA,opB,i=0;
    char operandoA,operandoB,aux;
    int auxReg,offset,secReg;
    char *funciones2OP[13] = {"MOV","ADD","SUB","SWAP","MUL","DIV","CMP","SHL","SHR","AND","OR","XOR","RND"};
    char *funciones1OP[11] = {"SYS","JMP","JZ","JP","JN","JNZ","JNP","JNN","LDL","LDH","NOT"};
    char *funciones0OP[1] = {"STOP"};
    char *Reg[16]={"CS","DS"," "," "," ","IP"," "," ","CC","AC","EAX","EBX","ECX","EDX","EEX","EFX"};
    char *RegHigh[16]={"CS","DS"," "," "," ","IP"," "," ","CC","AC","AH","BH","CH","DH","EH","FH"};
    char *RegLow[16]={"CS","DS"," "," "," ","IP"," "," ","CC","AC","AL","BL","CL","DL","EL","FL"};
    char *RegX[16]={"CS","DS"," "," "," ","IP"," "," ","CC","AC","AX","BX","CX","DX","EX","FX"};
    char mascara0Operando= 0xFF,mascara2Operando= 0x10;
    while( i< MV.segmento[0]){
        aux=MV.memoria[i];
        printf("[%04X] %02X ",i,(unsigned char)aux);
        i++;
        opB=0;
        opA=0;
        operandoB= (int)((( aux ^ 0b11000000 ) & 0b11000000) >> 6);
        for(int j=0;j<operandoB;j++){
            printf("%02X ",(unsigned char)MV.memoria[i]);
            opB = (opB << 8 ) | (MV.memoria[i++] & 0x000000FF);
        }
        operandoA= (int)((( aux ^ 0b00110000 ) & 0b00110000) >> 4);
        for(int j=0;j<operandoA;j++){
            printf("%02X ",(unsigned char)MV.memoria[i]);
            opA = (opA << 8 ) | (MV.memoria[i++] &0x000000FF);
        }
        printf("| ");
        if((aux & mascara2Operando) == 0){
            aux=aux&0x0F;
            printf("%s  ",funciones2OP[(int)aux]);
            if(operandoA==3){
                auxReg = opA>>16;
                offset = opA & 0x0000FFFF;
                if(auxReg!=1)
                    printf("[%s + %d], ",Reg[auxReg],offset);
                else
                    printf("[%d], ",offset);
            }
            else if (operandoA==1){
                secReg =(opA & 0x000000F0) >> 4;
                auxReg=opA & 0x0000000F;
                if(secReg==0)
                    printf("%s, ",Reg[auxReg]);
                else
                    if(secReg==1)
                        printf("%s, ",RegLow[auxReg]);
                    else
                        if(secReg==2)
                            printf("%s, ",RegHigh[auxReg]);
                        else
                            printf("%s, ",RegX[auxReg]);
            }
            else{
                printf("%d, ",opA);
            }
            if(operandoB==3){
                auxReg = opB>>16;
                offset = opB & 0x0000FFFF;
                if(auxReg!=1)
                    printf("[%s + %d]\n",Reg[auxReg],offset);
                else
                    printf("[%d]\n",offset);
            }
            else if (operandoB==1){
                secReg =(opB & 0x000000F0) >> 4;
                auxReg=opB & 0x0000000F;
                if(secReg==0)
                    printf("%s\n",Reg[auxReg]);
                else
                    if(secReg==1)
                        printf("%s\n",RegLow[auxReg]);
                    else
                        if(secReg==2)
                            printf("%s\n",RegHigh[auxReg]);
                        else
                            printf("%s\n",RegX[auxReg]);
            }
            else{
                printf("%d\n",opB);
            }
        }
        else
            if((aux & mascara0Operando) == mascara0Operando){
                printf("%s\n",funciones0OP[~aux]);
            }
        else{
            aux=aux&0x0F;
            printf("%s  ",funciones1OP[(int)aux]);
            if(operandoB==3){
                auxReg = opB>>16;
                offset = opB & 0x0000FFFF;
                if(auxReg!=1)
                    printf("[%s + %d]\n",Reg[auxReg],offset);
                else
                    printf("[%d]\n",offset);
            }
            else if (operandoB==1){
                secReg =(opB & 0x000000F0) >> 4;
                auxReg=opB & 0x0000000F;
                if(secReg==0)
                    printf("%s\n",Reg[auxReg]);
                else
                    if(secReg==1)
                        printf("%s\n",RegLow[auxReg]);
                    else
                        if(secReg==2)
                            printf("%s\n",RegHigh[auxReg]);
                        else
                            printf("%s\n",RegX[auxReg]);
            }
            else{
                printf("%04X\n",opB);
            }
        }
    }
}