#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"cJson.h"

typedef struct AdjListNode{
    int vertex;
    struct AdjListNode* next;
}AdjListNode;

typedef struct Node{
    AdjListNode* head;
}Node;

typedef struct Graph{
    int numVertices;
    Node* array;
}Graph;

