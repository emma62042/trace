#include <stdlib.h>
#include <stdio.h>

typedef struct req {
		double arrivalTime;		//抵達時間
		unsigned devno;			//裝置編號(預設為0)
		unsigned long diskBlkno;//Block編號(根據Disksim格式)
		unsigned reqSize;		//Block連續數量(至少為1)
		unsigned reqFlag;		//讀:1;寫:0
		unsigned userno;		//使用者編號(1~n)       
		double responseTime;	//反應時間(初始為0)
	} REQ;

int main(int argc, char *argv[]){
	if (argc != 3) {
    	fprintf(stderr, "usage: %s <Input trace file> <Output trace file>\n", argv[0]);
    	exit(1);
    }

    FILE *input, *output;
    input = fopen(argv[1], "r");
    output = fopen(argv[2], "w");

    REQ *tmp;
    tmp = calloc(1, sizeof(REQ));

    int newdiskBlkno=0, newreqSize=0;
    unsigned max_sectornum=0, min_sectornum=1351642016;
    unsigned asu;//Application specific unit (ASU)for Financial1,2
    char Flag;
    printf("Analyzing.......");
    while (!feof(input)) {
    	fscanf(input, "%u,%lu,%u,%c,%lf", &asu, &tmp->diskBlkno, &tmp->reqSize, &Flag, &tmp->arrivalTime);
    	
        if(asu != 0)
        {
            continue;
        }

        newreqSize=tmp->reqSize/4096;//(/4096=4K=page)
        if(tmp->reqSize%4096 > 0)
        {
            newreqSize=newreqSize+1;
        }
        newreqSize=newreqSize*8;
//-----------------------------------------------
        if(Flag == 'w')
        {
            tmp->reqFlag=0;
        }
        else if(Flag == 'r')
        {
            tmp->reqFlag=1;
        }
//----------------------------------------------------
        if(tmp->diskBlkno%8 > 0)
        {
            newdiskBlkno = tmp->diskBlkno/8;
            tmp->diskBlkno = newdiskBlkno * 8;
        }
//------------------------------------------------------
        if(tmp->diskBlkno > 10000000)
        {
            continue;
        }

        if(tmp->diskBlkno > max_sectornum)
        {
            max_sectornum = tmp->diskBlkno;
        }
        if(tmp->diskBlkno < min_sectornum)
        {
            min_sectornum = tmp->diskBlkno;
        }



    	fprintf(output, "%lf\t%u\t%lu\t%u\t%u\n", tmp->arrivalTime, 0, tmp->diskBlkno, newreqSize, tmp->reqFlag);
    }

    printf("[Finish]\n");
    printf("max_sectornum=%u\n", max_sectornum);
    printf("min_sectornum=%u\n", min_sectornum);
    fclose(input);
    fclose(output);

    return 0;
}
