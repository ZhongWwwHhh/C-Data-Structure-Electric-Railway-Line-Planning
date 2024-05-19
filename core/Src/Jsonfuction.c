#include "Jsonfunction.h"

//读取txt文件
FILE* READ_TXT(char* filename)
{
    FILE* file;
    file=fopen(filename,"r");
    if(file==NULL)
    {
        printf("Error: File not found!\n");
        exit(1);
    }
    return file;
}
//从文件指针指向的文件中读取一行，逐字符读取，精确内存分配 
char* READ_LINE(FILE* file)
{
    if(file==NULL)
    {
        printf("Error: File pointer is NULL!\n");
        exit(1);
    }
    char* line=NULL;
    int linesize=0;
    int ch;
    while((ch=fgetc(file))!=EOF)
    {
        char*temp=realloc(line,linesize+1);
        if(temp==NULL)
        {
            printf("Error: Memory allocation failed!\n");
            exit(1);
        }
        line=temp;
        line[linesize++]=ch;
        if(ch=='\n')
        {
            break;
        }
    }
    line=realloc(line,linesize+1);
        if(line==NULL)
        {
            printf("Error: Memory allocation failed!\n");
            exit(1);
        }
        line[++linesize]='/0';
        //printf("%s\n",line);
    return line;
}
//释放内存
void FREE_FILE(FILE* file)
{
    if(file!=NULL)
    {
        free(file);
        fclose(file);
    }
}


