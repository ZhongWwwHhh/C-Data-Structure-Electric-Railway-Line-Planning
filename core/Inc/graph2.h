#ifndef GRAPH2_H
#define GRAPH2_H

typedef struct AdjListNode
{
    int dest;
    int weight;
    struct AdjListNode *next;
} AdjListNode;

typedef struct AdjList
{
    AdjListNode *head;
} AdjList;

typedef struct Graph
{
    int V;
    AdjList *array;
} Graph;

Graph *createGraph(int V);
void insertEdge(Graph *graph, int src, int dest, int weight);
void printGraph(Graph *graph);

typedef struct MinHeapNode
{
    int v;
    int dist;
} MinHeapNode;

typedef struct MinHeap
{
    int size;
    int capacity;
    int *pos;
    MinHeapNode **array;
} MinHeap;

MinHeapNode *newMinHeapNode(int v, int dist);
MinHeap *createMinHeap(int capacity);
void swapMinHeapNode(MinHeapNode **a, MinHeapNode **b);
void minHeapify(MinHeap *minHeap, int idx);
int isEmpty(MinHeap *minHeap);
MinHeapNode *extractMin(MinHeap *minHeap);
void decreaseKey(MinHeap *minHeap, int v, int dist);
int isInMinHeap(MinHeap *minHeap, int v);
int *dijkstra(Graph *graph, int src, int dest);

#endif // GRAPH2_H
