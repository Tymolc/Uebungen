/* Betriebssystem & Middleware
 *
 * Betriebssysteme I WS 2014/2015
 *
 * Uebung 2.5
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#ifdef UNICODE
#pragma message("!!! Unicode is enabled. Be carefull using getDescription with none-UNICODE strings. !!!")
#endif
#define XSIZE 500
#define YSIZE 500
#include "algorithm.h"
#include <windows.h>
#include <math.h>

// used for passing data to worker threads
struct threadData {
    char *buffer;
    int firstline, lastline;
    HANDLE mutex;
};

// used for passing data to helper threads
struct createThreadData {
    char *buffer;
    int firstline, lastline, numOfThreads;
    HANDLE mutex;
};

DWORD WINAPI calcThread( LPVOID lpParam ) 
{
    char bgr[3];
    int x,y,count;
    struct threadData Data = *(struct threadData*) lpParam;
	
	printf("new thread starting at line %d, calculating %d values.\n", Data.firstline, (Data.lastline - Data.firstline)*YSIZE);
    for(y=Data.lastline;y>=Data.firstline;y--)
    {
        for(x=0;x<XSIZE;x++)
        {
            getColorValuesAt(x * (2.0 / XSIZE) - 1.5, y * (2.0 / YSIZE) - 1.0,&bgr[2],&bgr[1],&bgr[0]);	//calculate color for each pixel
            // wait for mutex
            if ( WaitForSingleObject( Data.mutex, INFINITE ) == WAIT_OBJECT_0 ) 
            { 
                // write to buffer using modified formula for pixel position
                memcpy(&(Data.buffer[(499 - y)*1500 + x*3]), bgr, 3);
                // release mutex
                ReleaseMutex( Data.mutex ); 
            }
        }
    }
    return 0;
}

DWORD WINAPI createCalcThreads( LPVOID lpParam ) 
{
    struct createThreadData Data = *(struct createThreadData*) lpParam;

    int firstline, lastline, numOfThreads;
    int numlines;
    float linesPerThread;
    int i;

    // arrays have to be initialized with constant size
    HANDLE Array_Of_Thread_Handles[64];
    struct threadData DataForThread[64];
    DWORD threadId;

    // if there are no worker threads to start, there is nothing to do
    if(Data.numOfThreads == 0){
        return 0;
    }

    firstline = Data.firstline;
    lastline = Data.lastline;
    numOfThreads = Data.numOfThreads;

    numlines = lastline-firstline+1;
    linesPerThread = (float)numlines/(float)numOfThreads;

    // fill each worker thread
    for(i = 0; i<numOfThreads; i++){
        DataForThread[i].buffer = Data.buffer;
        DataForThread[i].mutex = Data.mutex;
        DataForThread[i].firstline = (int)(firstline + linesPerThread*i);
        DataForThread[i].lastline = (int)(firstline + linesPerThread*(i+1) - 1);
        if(i == numOfThreads-1)
            if(firstline + linesPerThread*(i+1) - 1 < lastline)
                DataForThread[i].lastline++;
        // create thread
        Array_Of_Thread_Handles[i] = CreateThread( NULL, 0, 
           calcThread, &(DataForThread[i]), 0, &threadId);
    }

    // wait for all threads to have finished
    WaitForMultipleObjects( numOfThreads, 
        Array_Of_Thread_Handles, TRUE, INFINITE);

    // close all handles
    for(i = 0; i<64; i++)
        CloseHandle(Array_Of_Thread_Handles[i]);

    return 0;
}

int main(int argc, char *argv[])
{
    FILE *fd;
    int len,x,y;
    char *dsc;
    short svalue;
    int   lvalue;
    unsigned char header[54],*ptr=&header[0];

    struct createThreadData DataForThread[2];
    HANDLE Array_Of_Thread_Handles[2];
    HANDLE mutex;
    DWORD threadId;
    char *buffer;
    int numberOfThreads;

    int num;
    char *file;

    // buffer for writing image data to
    buffer = (char*) malloc(XSIZE*YSIZE*3);
    if(buffer == NULL)
    {
        perror("malloc");
        exit(1);
    }

    // check arguments
    if(argc != 3) {
        printf("Invalid number of arguments. Has to be [ProgramName NumberOfThreads OutputFile].\n");
        exit(0);
    }

    num = atoi(argv[1]);
    if(num < 1 || num>128){
        printf("Invalid number of threads. Has to be >0 and <129.\n");
        exit(0);
    }
    numberOfThreads = num;
        
    file = argv[2];
    if(strlen(argv[2]) < 5){
        printf("Invalid filename. Has to be of format .bmp.\n");
        exit(0);
    }

    getDescription(NULL,&len);
    if(NULL==(dsc=(char*)malloc(sizeof(char)*len)))
    {
        perror("malloc");
        exit(1);
    }
	getDescription(&dsc,&len);
    printf("Calculate %s %d\n",dsc,getId());
    fd=fopen(file,"wb+");
    if(NULL==fd)
    {
        perror("open"); exit(1);
    }
        
    svalue=0x4d42;
    memcpy(ptr,&svalue,2);//signatur
    ptr+=2;
    lvalue=XSIZE*YSIZE*3+54;
    memcpy(ptr,&lvalue,4); //filesize
    ptr+=4;
    lvalue=0;
    memcpy(ptr,&lvalue,4);//reserved
    ptr+=4;
    lvalue=54;
    memcpy(ptr,&lvalue,4);//image offset
    ptr+=4;
    lvalue=40;
    memcpy(ptr,&lvalue,4);//size of header follows
    ptr+=4;
    lvalue=XSIZE;
    memcpy(ptr,&lvalue,4);//with of image
    ptr+=4;
    lvalue=YSIZE;
    memcpy(ptr,&lvalue,4); //height of image
    ptr+=4;
    svalue=1;
    memcpy(ptr,&svalue,2); //number of planes
    ptr+=2;
    svalue=24;
    memcpy(ptr,&svalue,2); //number of pixel
    ptr+=2;
    lvalue=0; //compression
    memcpy(ptr,&lvalue,4); //compression
    ptr+=4;
    lvalue=XSIZE*YSIZE*3; 
    memcpy(ptr,&lvalue,4); //size of image
    ptr+=4;
    lvalue=0;
    memcpy(ptr,&lvalue,4); //xres  
    ptr+=4;
    lvalue=0;
    memcpy(ptr,&lvalue,4); //yres
    ptr+=4;
    lvalue=0;
    memcpy(ptr,&lvalue,4); //number of colortables
    ptr+=4;
    lvalue=0;
    memcpy(ptr,&lvalue,4); //number of important colors
    ptr+=4;
    
    len=fwrite(header,1,sizeof(header),fd); //write header
    
    if(-1==len || len!=sizeof(header))
    {
        perror("write");
        exit(2);
    }

    // create mutex
    mutex = CreateMutex( 
        NULL,              // default security attributes
        FALSE,             // initially not owned
        NULL);             // unnamed mutex

    if (mutex == NULL) 
    {
        printf("CreateMutex error: %d\n", GetLastError());
        return 1;
    }

    // set data for helper threads
    DataForThread[0].buffer = buffer;
    DataForThread[0].mutex = mutex;
    DataForThread[0].numOfThreads = ceil((float)numberOfThreads / 2.0);
    DataForThread[0].firstline = 0;
    DataForThread[0].lastline = (int)((float)DataForThread[0].numOfThreads/(float)numberOfThreads * 499.0);
    DataForThread[1].buffer = buffer;
    DataForThread[1].mutex = mutex;
    DataForThread[1].numOfThreads = numberOfThreads - DataForThread[0].numOfThreads;
    DataForThread[1].firstline = DataForThread[0].lastline + 1;
    DataForThread[1].lastline = 499;

    // create thread 1
    Array_Of_Thread_Handles[0] = CreateThread( NULL, 0, 
           createCalcThreads, &(DataForThread[0]), 0, &threadId);
    
    // create thread 2
    Array_Of_Thread_Handles[1] = CreateThread( NULL, 0, 
           createCalcThreads, &(DataForThread[1]), 0, &threadId);
    
    // wait until all threads have terminated
    WaitForMultipleObjects( 2, 
        Array_Of_Thread_Handles, TRUE, INFINITE);

    // close handles
    CloseHandle(Array_Of_Thread_Handles[0]);
    CloseHandle(Array_Of_Thread_Handles[1]);
    CloseHandle(mutex);

    // write the collected data to the image
    len=fwrite(buffer,1,XSIZE*YSIZE*3,fd);
    
    if(-1==len || len!=XSIZE*YSIZE*3)
    {
        perror("write");
        exit(2);
    }

    fclose(fd);
    
}
