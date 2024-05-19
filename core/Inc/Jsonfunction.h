#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"cJson.h"
//读取txt文件
FILE* READ_TXT(char* filename);
//从文件指针指向的文件中读取一行，逐字符读取，精确内存分配
char* READ_LINE(FILE* file);
//释放内存
void FREE_FILE(FILE* file);

