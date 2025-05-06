#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>

#define SIZEKEY 6
#define SIZEACCESSBITS 3
#define ACCESSBYTE1 54
#define ACCESSBYTE2 55
#define ACCESSBYTE3 56

/*
return true if nblock of the sector is a valueblock
*/
bool printParserAccessBits(const uint8_t byte6,const uint8_t byte7,const uint8_t byte8, const int nBlock){
    uint8_t excluserFirstHalf=0b00000001;
    uint8_t excluserSecondHalf=0b00010000;
    bool c2,c1,c3,nc2,nc1,nc3;
    excluserSecondHalf<<=nBlock;
    excluserFirstHalf<<=nBlock;
    nc1=(byte6&excluserFirstHalf);
    c1=(byte7&excluserSecondHalf);
    nc3=(byte7&excluserFirstHalf);
    c3=(byte8&excluserSecondHalf);
    nc2=(byte6&excluserSecondHalf);
    c2=(byte8&excluserFirstHalf);
    if(c1&&nc1){
        printf("\n\nError in access bits of the dump, c1 \n\n");
        exit(1);
    }
    if(c2&&nc2){
        printf("\n\nError in access bits of the dump, c2 \n\n");
        exit(1);
    }
    if(c3&&nc3){
        printf("\n\nError in access bits of the dump, c3 \n\n");
        exit(1);
    }
      if(nBlock==3){
        //Sector trailer
        //|0 0 0|
        if(nc1&&nc2&&nc3){
            printf(" %u %u %u | |[A]wA | [Acc]rA   | [B]rAwB | *",c1,c2,c3);
            return false;
        }
        //|0 1 0|
        if(nc1&&c2&&nc3){
            printf(" %u %u %u | |[A]-- | [Acc]rA    | [B]rA   | *",c1,c2,c3);
            return false;
        }
        //|1 0 0|
        if(c1&&nc2&&nc3){
            printf(" %u %u %u | |[A]wB | [Acc]rAB   | [B]wB   |",c1,c2,c3);
            return false;
        }
        //|1 1 0|
        if(c1&&c2&&nc3){
            printf(" %u %u %u | |[A]-- | [Acc]rAB   | [B]--   |",c1,c2,c3);
            return true;
        }
        //|1 1 0|
        if(nc1&&nc2&&c3){
            printf(" %u %u %u | |[A]wA | [Acc]rAwA  | [B]rAwA | Transport Configuration *",c1,c2,c3);
            return true;
        }
        //|0 1 1|
        if(nc1&&c2&&c3){
            printf(" %u %u %u | |[A]wB | [Acc]rABwB | [B]wB   |",c1,c2,c3);
            return false;
        }
        //|1 0 1|
        if(c1&&nc2&&c3){
            printf(" %u %u %u | |[A]-- | [Acc]rABwB | [B]--   |",c1,c2,c3);
            return false;
        }
        //|1 1 1|
        if(c1&&c2&&c3){
            printf(" %u %u %u | |[A]wB | [Acc]rABwB | [B]wB   |",c1,c2,c3);
            return false;
        }
    }else{
        //|0 0 0|
        if(nc1&&nc2&&nc3){
            printf(" %u %u %u | |AB|AB|AB|AB| Transport configuration",c1,c2,c3);
            return false;
        }
        //|0 1 0|
        if(nc1&&c2&&nc3){
            printf(" %u %u %u | |AB|--|--|--|",c1,c2,c3);
            return false;
        }
        //|1 0 0|
        if(c1&&nc2&&nc3){
            printf(" %u %u %u | |AB|-B|--|--|",c1,c2,c3);
            return false;
        }
        //|1 1 0|
        if(c1&&c2&&nc3){
            printf(" %u %u %u | |AB|-B|-B|AB| Value block with recharging",c1,c2,c3);
            return true;
        }
        //|0 0 1|
        if(nc1&&nc2&&c3){
            printf(" %u %u %u | |AB|--|--|AB| Value block non-rechargeable",c1,c2,c3);
            return true;
        }
        //|0 1 1|
        if(nc1&&c2&&c3){
            printf(" %u %u %u | |-B|-B|--|--|",c1,c2,c3);
            return false;
        }
        //|1 0 1|
        if(c1&&nc2&&c3){
            printf(" %u %u %u | |-B|--|--|--|",c1,c2,c3);
            return false;
        }
        //|1 1 1|
        if(c1&&c2&&c3){
            printf(" %u %u %u | |--|--|--|--|",c1,c2,c3);
            return false;
        }
	printf("Ops... an error occurred");
    }
    return false;
}

int main(int argc, char *argv[])
{
    int opt,sector,block,i,asciiCount;
    const int bytesPerBlock=16;
    const int sectors=16;
    const int blockspersector=4;
    uint8_t sectorIteration[bytesPerBlock*blockspersector];
    char *path=NULL;
    //enable/disable the printg of block in ascci
    bool ascci=true;

    while ((opt = getopt(argc, argv, "I:hva")) != -1) {
        switch(opt) {
            case 'I':
                path=optarg;
                break;
            case 'h':
                printf("Use: ./program -I file [-h] [-v] [-a to not stamp ascii carachters]\n");
                return 0;
            case 'v':
                printf("Version 0.1\n");
                return 0;
            case 'a':
                ascci=false;
                break;
            default:
                printf("Use: ./program -I file [-h] [-v] [-a to not stamp ascii carachters]\n");
                return 0;
                break;
            case ':':  
                printf("option needs a value\n");  
                break;
        }
    }
    if(!path){
        printf("Input file needed\n Use: ./program -I file [-h] [-v]\n");
        return 0;
    }

    FILE *file=fopen(path,"rb");
    if(!file){
        printf("Input file not found\n");
        return 0;
    }
    if(ascci)
        printf("Sect Block|                                                 | block in ascci |acc bit| |R |W |I |D |\n");
    else
        printf("Sect Block|                                                 |acc bit| |R |W |I |D |\n");
    
    for(sector=0;sector<sectors;sector++){
        printf("[%02d] ",sector);
        
            if(fread(sectorIteration,1,bytesPerBlock*blockspersector,file) != bytesPerBlock*blockspersector) {
                perror("Errore lettura file");
                fclose(file);
                return 1;
            }
        for(block=0,i=0,asciiCount=0;block<blockspersector;block++){    
            
            printf("[%02d] | ",block);
            
            if(block==3){
                printf("\033[1;31m");
                for(;i<SIZEKEY+(block*bytesPerBlock);i++)
                    printf("%02X ",sectorIteration[i]);
                printf("\033[1;32m");
                for(;i<(SIZEKEY+SIZEACCESSBITS)+(block*bytesPerBlock);i++)
                    printf("%02X ",sectorIteration[i]);
                printf("\033[0m%02X \033[1;31m",sectorIteration[i]);
                i++;
                for(;i<(SIZEKEY+SIZEKEY+SIZEACCESSBITS+1)+(block*bytesPerBlock);i++)
                    printf("%02X ",sectorIteration[i]);

            }else{
                if(!sector&&!block)
                    printf("\033[1;34m");
                for(;i<bytesPerBlock*(block+1);i++)
                    printf("%02X ",sectorIteration[i]);
            }
                
                printf("\033[0m");
                printf("|");

                //print block in ascii
                if(ascci){
                    for(;asciiCount<bytesPerBlock*(block+1);asciiCount++)
                        printf("%c",(sectorIteration[asciiCount] >= 32 && sectorIteration[asciiCount] <= 126) ? sectorIteration[asciiCount] : '.');
                    printf("|");
                }
                printParserAccessBits(sectorIteration[ACCESSBYTE1],sectorIteration[ACCESSBYTE2],sectorIteration[ACCESSBYTE3],block);
            
            if(block==3)
                printf("\n");
            else
                printf("\n     ");
        }
    }
    printf("* For this access condition KeyB is readable and may be used for data\nR/r->Read\nW/w->Write\nI->Increment\nD->Decrement, Transfer, Restore\n");
    return 0;
}
