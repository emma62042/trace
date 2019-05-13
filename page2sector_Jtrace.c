#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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
	if (argc != 2) {
    	fprintf(stderr, "usage: %s <Input trace file> <Output trace file>\n", argv[0]);
    	exit(1);
    }
    char msg[50];
    strcpy(msg,"page2sector_");
    FILE *input, *output;
    input = fopen(argv[1], "r");
    strcat(msg,argv[1]);
    output = fopen(msg, "w");

    REQ *tmp;
    int usernum1, usernum2;
    tmp = calloc(1, sizeof(REQ));
    fscanf(input, "%d%d", &usernum1, &usernum2);
    int newdiskBlkno=0, newreqSize=0;
    printf("Analyzing.......");
    while (!feof(input)) {
    	fscanf(input, "%lf%u%lu%u%u%u", &tmp->arrivalTime, &tmp->devno, &tmp->diskBlkno, &tmp->reqSize, &tmp->reqFlag, &tmp->userno);
    	newdiskBlkno=tmp->diskBlkno * 8;
    	fprintf(output, "%lf\t%u\t%lu\t%u\t%u\n", tmp->arrivalTime, tmp->devno, newdiskBlkno, 8, tmp->reqFlag);
    }

    printf("[Finish]\n");
    fclose(input);
    fclose(output);

    return 0;
}
