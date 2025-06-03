// MIT License
// Copyright (c) 2025 M1ke324
// Permission is granted to use, modify, and distribute this code freely.
// See the LICENSE file for full license text.


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

void printParserAccessBits(const uint8_t byte6,const uint8_t byte7,const uint8_t byte8, const int nBlock,bool verbose){
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
            printf(" %u %u %u | |[A]wA | [Acc]rA   | [B]rAwB |",c1,c2,c3);
			if(verbose){	
				printf(" *");
			}
            return;
        }
        //|0 1 0|
        if(nc1&&c2&&nc3){
            printf(" %u %u %u | |[A]-- | [Acc]rA    | [B]rA   |",c1,c2,c3);
            if(verbose){	
				printf(" *");
			}
			return;
        }
        //|1 0 0|
        if(c1&&nc2&&nc3){
            printf(" %u %u %u | |[A]wB | [Acc]rAB   | [B]wB   |",c1,c2,c3);
            return;
        }
        //|1 1 0|
        if(c1&&c2&&nc3){
            printf(" %u %u %u | |[A]-- | [Acc]rAB   | [B]--   |",c1,c2,c3);
            return;
        }
        //|1 1 0|
        if(nc1&&nc2&&c3){
            printf(" %u %u %u | |[A]wA | [Acc]rAwA  | [B]rAwA |",c1,c2,c3);
            if(verbose){	
				printf(" Transport Configuration *");
			}
			return;
        }
        //|0 1 1|
        if(nc1&&c2&&c3){
            printf(" %u %u %u | |[A]wB | [Acc]rABwB | [B]wB   |",c1,c2,c3);
            return;
        }
        //|1 0 1|
        if(c1&&nc2&&c3){
            printf(" %u %u %u | |[A]-- | [Acc]rABwB | [B]--   |",c1,c2,c3);
            return;
        }
        //|1 1 1|
        if(c1&&c2&&c3){
            printf(" %u %u %u | |[A]wB | [Acc]rABwB | [B]wB   |",c1,c2,c3);
            return;
        }
		printf("Ops... an error occurred");
		exit(1);
    }else{
        //|0 0 0|
        if(nc1&&nc2&&nc3){
            printf(" %u %u %u | |AB|AB|AB|AB|",c1,c2,c3);
            if(verbose){	
				printf(" Transport Configuration");
			}
			return;
        }
        //|0 1 0|
        if(nc1&&c2&&nc3){
            printf(" %u %u %u | |AB|--|--|--|",c1,c2,c3);
            return;
        }
        //|1 0 0|
        if(c1&&nc2&&nc3){
            printf(" %u %u %u | |AB|-B|--|--|",c1,c2,c3);
            return;
        }
        //|1 1 0|
        if(c1&&c2&&nc3){
            printf(" %u %u %u | |AB|-B|-B|AB|",c1,c2,c3);
            if(verbose){	
				printf(" Value block with recharging");
			}
			return;
        }
        //|0 0 1|
        if(nc1&&nc2&&c3){
            printf(" %u %u %u | |AB|--|--|AB|",c1,c2,c3);
            if(verbose){	
				printf(" Value block non-rechargeable");
			}
			return;
        }
        //|0 1 1|
        if(nc1&&c2&&c3){
            printf(" %u %u %u | |-B|-B|--|--|",c1,c2,c3);
            return;
        }
        //|1 0 1|
        if(c1&&nc2&&c3){
            printf(" %u %u %u | |-B|--|--|--|",c1,c2,c3);
            return;
        }
        //|1 1 1|
        if(c1&&c2&&c3){
            printf(" %u %u %u | |--|--|--|--|",c1,c2,c3);
            return;
        }
		printf("Ops... an error occurred");
		exit(1);
    }
    return;
}
void searchValueBlock(uint8_t *block,bool verbose){
	//Check if the block respect the value block scheme
	if(  (*(uint32_t *)block == ~*((uint32_t *)block+1))
		&& (*((uint32_t *)block+1) == ~*((uint32_t *)block+2) )&&
		( (*(block+12) == (uint8_t)~*(block+13))
		&& (*(block+13) == (uint8_t)~*(block+14))
		&& (*(block+14) == (uint8_t)~*(block+15)) ) ){
			printf(" [Value: %d] [Address: %02X]",*(uint32_t *)block,*(block+12));
			return;
	}
	if(verbose)
		printf(" [Not a value block]");
}
int main(int argc, char *argv[])
{
    int opt,sector,block,i,asciiCount;
    const int bytesPerBlock=16;
    const int sectors=16;
    const int blockspersector=4;
	//buffer to save the sector
    uint8_t sectorIteration[bytesPerBlock*blockspersector];
	//path of the dump file
    char *path=NULL;
    //enable/disable the print of block in ascci
    bool ascci=true;
	//enable/disable the search of a valuse block
    bool searchValueBlockBool=false;
	//Verbosity	
	bool verbose=false;

    while ((opt = getopt(argc, argv, "I:hvabV")) != -1) {
        switch(opt) {
            case 'I':
				//file
                path=optarg;
                break;
            case 'v':
				//version
                printf("Version 0.2\n");
                return 0;
			case 'V':
				//Verbosity
				verbose=true;
				break;
			case 'b':
				//enable the search of a valueblock
				searchValueBlockBool=true;
				break;
			case 'a':
				//disable the print of block in ascii
                ascci=false;
                break;
			case 'h':
				//help
				printf("Trailer block keyword meanings:\nR/r->Read\nW/w->Write\nI->Increment\nD->Decrement, Transfer, Restore\n\n");
				//fall through
           	default:
                printf("Use: ./mfcd -I file [-h help] [-V verbose] [-v version] [-a to not stamp ascii carachters][-b scan for value block]\n");
                return 0;
                break;
            case ':':  
                printf("option needs a value\n");  
                break;
        }
    }
    if(!path){
        printf("Input file needed\n\n");
        printf("Use: ./mfcd -I file [-h] [-v] [-V verbose] [-a to not stamp ascii carachters][-b scan for value block]\n");
        return 0;
    }

    FILE *file=fopen(path,"rb");
    if(!file){
        printf("Input file not found\n");
        return 0;
    }
	//print table head
    if(ascci)
        printf("Sect Block|                                                 | block in ascci |acc bit| |R |W |I |D |\n");
    else
        printf("Sect Block|                                                 |acc bit| |R |W |I |D |\n");
   //sectors cycle 
    for(sector=0;sector<sectors;sector++){
		//print the sector number
        printf("[%02d] ",sector);
       	//read a sector 
        if(fread(sectorIteration,1,bytesPerBlock*blockspersector,file) != (size_t)bytesPerBlock*blockspersector) {
            perror("Errore lettura file");
            fclose(file);
			return 1;
        }
		//blocks cycle
        for(block=0,i=0,asciiCount=0;block<blockspersector;block++){    
           //print the block number 
            printf("[%02d] | ",block);
           //if the block is a sector trailer use different color 
            if(block==3){
                printf("\033[1;31m");
				//KEY A
                for(;i<SIZEKEY+(block*bytesPerBlock);i++)
                    printf("%02X ",sectorIteration[i]);
                printf("\033[1;32m");
				//ACCESS BITS
                for(;i<(SIZEKEY+SIZEACCESSBITS)+(block*bytesPerBlock);i++)
                    printf("%02X ",sectorIteration[i]);
				//DATA BYTE
                printf("\033[0m%02X \033[1;31m",sectorIteration[i]);
                i++;
				//KEY B
                for(;i<(SIZEKEY+SIZEKEY+SIZEACCESSBITS+1)+(block*bytesPerBlock);i++)
                    printf("%02X ",sectorIteration[i]);

            }else{
				//clear the color if is not the first block of the first sector the must be blue
                if(!sector&&!block)
                    printf("\033[1;34m");
				//print the byte of the normal block 
                for(;i<bytesPerBlock*(block+1);i++)
                    printf("%02X ",sectorIteration[i]);
            }
			//clean olor
            printf("\033[0m|");

            //print block in ascii
            if(ascci){
                for(;asciiCount<bytesPerBlock*(block+1);asciiCount++)
                    printf("%c",(sectorIteration[asciiCount] >= 32 && sectorIteration[asciiCount] <= 126) ? sectorIteration[asciiCount] : '.');
				printf("|");
			}
			//print the accces bits
            printParserAccessBits(sectorIteration[ACCESSBYTE1],sectorIteration[ACCESSBYTE2],sectorIteration[ACCESSBYTE3],block,verbose);
           
		if(searchValueBlockBool&&block!=3&&(block||sector))	
				searchValueBlock(&sectorIteration[block*bytesPerBlock],verbose);

            if(block==3)
                printf("\n");
            else
                printf("\n     ");
        }
    }
	if(verbose)	
    	printf("* For this access condition KeyB is readable and may be used for data\nR/r->Read\nW/w->Write\nI->Increment\nD->Decrement, Transfer, Restore\n");
    return 0;
}
