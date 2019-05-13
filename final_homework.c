#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

double hitcount=0,reqcount=0;
FILE *fout;

typedef struct QNode
{
    struct QNode *prev,*next;
    unsigned blockNumber;
} QNode;

typedef struct Queue
{
    unsigned count;  
    unsigned numberOfFrames; 
    QNode *front,*rear;//第一個QNODE的位址 
} Queue;

Queue* createQueue(int numberOfFrames)
{
    Queue* queue=(Queue *)malloc(sizeof(Queue));
 
    queue->count=0;
    queue->front=queue->rear=NULL;

    queue->numberOfFrames=numberOfFrames;
 
    return queue;
}

QNode* newQNode(unsigned blockNumber)
{
    QNode* temp=(QNode *)malloc(sizeof(QNode));
    temp->blockNumber=blockNumber;
 
    temp->prev=temp->next=NULL;
 
    return temp;
}

int AreAllFramesFull(Queue* queue)
{
    return queue->count==queue->numberOfFrames;
}

int isQueueEmpty(Queue* queue)
{
    return queue->rear==NULL;
}

//刪掉Queue尾端 
void deQueue(Queue* queue)
{
    if(isQueueEmpty(queue))
        return;
 
    //只有一個node 先清queue開頭指標 
    if(queue->front==queue->rear)
        queue->front=NULL;
 	
 	//Queue最後指標指到前一個 
    QNode* temp=queue->rear;
    queue->rear=queue->rear->prev;
 	
 	//去掉最後node的後指標 
    if(queue->rear)  
        queue->rear->next=NULL;
 
    free(temp);
    queue->count--;
}

//Queue前面加一個Node 
void Enqueue(Queue* queue,unsigned blockNumber)
{
    if(AreAllFramesFull(queue))
    {
        deQueue(queue);
    }
 
    QNode* temp=newQNode(blockNumber);
    temp->next=queue->front;
 	
 	//queue是空的，把queue首尾都指到temp 
    if(isQueueEmpty(queue))
        queue->rear=queue->front=temp;
    else
    {
        queue->front->prev=temp;
        queue->front=temp;
	}

    queue->count++;
}

void ReferenceBlock(Queue* queue,unsigned blockNumber)
{
    QNode* reqBlock=queue->front;
    QNode* numBlock=NULL ;
    
    //目前QUEUE裡都沒有東西 
	if(reqBlock==NULL)
	{
		Enqueue( queue, blockNumber );
		reqBlock=queue->front;
		queue->rear=reqBlock;
        return;
	}
    
    //往下循序尋找 
    while(reqBlock!=queue->rear)
    {
    	//找到 
    	if(reqBlock->blockNumber==blockNumber)
    	{
    		numBlock=reqBlock;
    		break;
		}
		else
		{
			reqBlock=reqBlock->next;
		}
	}
	if(reqBlock==queue->rear&&reqBlock->blockNumber==blockNumber)
    {
    	numBlock=reqBlock;
	}
	
	//沒找到，加一個 
	if(numBlock==NULL)
	{
		Enqueue(queue,blockNumber);
	}
	else if(numBlock!=queue->front)
    {
    	hitcount++;
        printf("%d\n",numBlock->blockNumber);
        fprintf(fout, "%d\n",numBlock->blockNumber);

        numBlock->prev->next=numBlock->next;//把目標的前一個指到下一個 
        if(numBlock->next)
           numBlock->next->prev=numBlock->prev;//把目標的下一個指到目標的前一個

		//目標是最後一個 
        if(numBlock==queue->rear)
        {
           queue->rear=numBlock->prev;
           queue->rear->next=NULL;
        }

        numBlock->next=queue->front;
        numBlock->prev=NULL;
 
        numBlock->next->prev=numBlock;
        
        queue->front=numBlock;
    }
    else if(numBlock==queue->front)
    {
    	hitcount++;
        printf("%d\n",numBlock->blockNumber);
        fprintf(fout, "%d\n",numBlock->blockNumber);
	}
}
 

int main(int argc, char *argv[]){
    if (argc != 2) {
        fprintf(stderr, "usage: %s <Input trace file> \n", argv[0]);
        exit(1);
    }
	struct  timeval start;
    struct  timeval end;
        
    unsigned  long usec,sec,min,hour;
    
	gettimeofday(&start,NULL);
    
    double hit_rate=0;
	//131072 blocks

    Queue* q=createQueue(16000);//16mb=4096,32mb=8192,128mb=32768,512mb=131072 
 
    FILE *fp ;
    char str[20];
    fp= fopen(argv[1], "r");
    fout= fopen("hitnum.txt", "w");
    //fp=fopen( "/home/osnet/only_blkno_iozone-100K.txt", "r" );
    while(fgets(str,20,fp)!=NULL)//取得一個data 
    {
        reqcount++;
        int a=atoi(str);//轉int
        ReferenceBlock(q,a);
    }

    
    hit_rate=(hitcount/reqcount);
    printf("hit_rate=%f\n",hit_rate);

    fclose(fp);
    
   /* 
    printf("%d",q->front->blockNumber);
    printf("%d",q->front->next->blockNumber);
    printf("%d",q->front->next->next->blockNumber);
    printf("%d",q->front->next->next->next->blockNumber);
    */
    gettimeofday(&end,NULL);
    
    usec=end.tv_usec-start.tv_usec;
	sec=(end.tv_sec-start.tv_sec)-(((end.tv_sec-start.tv_sec)/60)*60);
	min=((end.tv_sec-start.tv_sec)/60)-((((end.tv_sec-start.tv_sec)/60)/60)*60);
	hour=((end.tv_sec-start.tv_sec)/60)/60;
    printf("\nthe time is %ld hour %ld min %ld sec %ld usec.\n",hour,min,sec,usec);
 
    return 0;
}
