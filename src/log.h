#pragma once
#include<stdio.h>
#define OPENLOGRECORD
#ifdef OPENLOGRECORD

#define LOG(... )  do{  printf( __VA_ARGS__); fprintf(testlog,__VA_ARGS__); }while (0);
#define OPEN_LOG(logfile,logname) FILE* logfile = fopen(logname,"w+");
#define CLOSE_LOG(logfile)  fclose(logfile);
#define LOGFILE testlog
#define LOGNAME "test.log"
 inline OPEN_LOG(LOGFILE,LOGNAME)
#else
#define LOG(... )  do{  printf( __VA_ARGS__); }while (0);
#define OPEN_LOG(logfile) 
#define CLOSE_LOG(logfile)  
#define LOGFILE
#define LOGNAME 
#endif 



#define MIN(a,b)    (((a)<(b))?(a):(b))

#define MAX(a,b)    (((a)>(b))?(a):(b))