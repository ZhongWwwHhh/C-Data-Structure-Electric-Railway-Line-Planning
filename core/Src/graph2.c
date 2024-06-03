#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "graph2.h"

// 创建一个新的邻接表节点
AdjListNode *createNewNode(int dest, int weight)
{
    AdjListNode *newNode = (AdjListNode *)malloc(sizeof(AdjListNode));
    newNode->dest = dest;
    newNode->weight = weight;
    newNode->next = NULL;
    return newNode;
}

// 创建一个新的图
Graph *createGraph(int V)
{
    Graph *graph = (Graph *)malloc(sizeof(Graph));
    graph->V = V;
    graph->array = (AdjList *)malloc(V * sizeof(AdjList));
    for (int i = 0; i < V; ++i)
    {
        graph->array[i].head = NULL;
    }
    return graph;
}

// 插入边到图中
void insertEdge(Graph *graph, int src, int dest, int weight)
{
    // 添加从 src 到 dest 的边
    AdjListNode *newNode = createNewNode(dest, weight);
    newNode->next = graph->array[src].head;
    graph->array[src].head = newNode;

    // 有向不加
    // 添加从 dest 到 src 的边（因为是无向图）
    // newNode = createNewNode(src, weight);
    // newNode->next = graph->array[dest].head;
    // graph->array[dest].head = newNode;
}

// 打印图
void printGraph(Graph *graph)
{
    for (int v = 0; v < graph->V; ++v)
    {
        AdjListNode *pCrawl = graph->array[v].head;
        printf("\n 顶点 %d 的邻接表\n head ", v);
        while (pCrawl)
        {
            printf("-> %d (权重 %d) ", pCrawl->dest, pCrawl->weight);
            pCrawl = pCrawl->next;
        }
        printf("\n");
    }
}

// 创建一个新的最小堆节点
MinHeapNode *newMinHeapNode(int v, int dist)
{
    MinHeapNode *minHeapNode = (MinHeapNode *)malloc(sizeof(MinHeapNode));
    minHeapNode->v = v;
    minHeapNode->dist = dist;
    return minHeapNode;
}

// 创建一个最小堆
MinHeap *createMinHeap(int capacity)
{
    MinHeap *minHeap = (MinHeap *)malloc(sizeof(MinHeap));
    minHeap->pos = (int *)malloc(capacity * sizeof(int));
    minHeap->size = 0;
    minHeap->capacity = capacity;
    minHeap->array = (MinHeapNode **)malloc(capacity * sizeof(MinHeapNode *));
    return minHeap;
}

// 交换两个节点的函数
void swapMinHeapNode(MinHeapNode **a, MinHeapNode **b)
{
    MinHeapNode *t = *a;
    *a = *b;
    *b = t;
}

// 最小堆化
void minHeapify(MinHeap *minHeap, int idx)
{
    int smallest, left, right;
    smallest = idx;
    left = 2 * idx + 1;
    right = 2 * idx + 2;

    if (left < minHeap->size &&
        minHeap->array[left]->dist < minHeap->array[smallest]->dist)
        smallest = left;

    if (right < minHeap->size &&
        minHeap->array[right]->dist < minHeap->array[smallest]->dist)
        smallest = right;

    if (smallest != idx)
    {
        // 节点位置在堆中的位置
        MinHeapNode *smallestNode = minHeap->array[smallest];
        MinHeapNode *idxNode = minHeap->array[idx];

        // 交换位置
        minHeap->pos[smallestNode->v] = idx;
        minHeap->pos[idxNode->v] = smallest;

        // 交换节点
        swapMinHeapNode(&minHeap->array[smallest], &minHeap->array[idx]);

        minHeapify(minHeap, smallest);
    }
}

// 检查最小堆是否为空
int isEmpty(MinHeap *minHeap)
{
    return minHeap->size == 0;
}

// 提取最小堆中的最小节点
MinHeapNode *extractMin(MinHeap *minHeap)
{
    if (isEmpty(minHeap))
        return NULL;

    // 存储根节点
    MinHeapNode *root = minHeap->array[0];

    // 替换根节点和最后一个节点
    MinHeapNode *lastNode = minHeap->array[minHeap->size - 1];
    minHeap->array[0] = lastNode;

    // 更新位置
    minHeap->pos[root->v] = minHeap->size - 1;
    minHeap->pos[lastNode->v] = 0;

    // 减少堆的大小
    --minHeap->size;
    minHeapify(minHeap, 0);

    return root;
}

// 减少最小堆中的距离值
void decreaseKey(MinHeap *minHeap, int v, int dist)
{
    int i = minHeap->pos[v];
    minHeap->array[i]->dist = dist;

    // 修正最小堆
    while (i && minHeap->array[i]->dist < minHeap->array[(i - 1) / 2]->dist)
    {
        minHeap->pos[minHeap->array[i]->v] = (i - 1) / 2;
        minHeap->pos[minHeap->array[(i - 1) / 2]->v] = i;
        swapMinHeapNode(&minHeap->array[i], &minHeap->array[(i - 1) / 2]);

        i = (i - 1) / 2;
    }
}

// 检查给定的顶点是否在最小堆中
int isInMinHeap(MinHeap *minHeap, int v)
{
    if (minHeap->pos[v] < minHeap->size)
        return 1;
    return 0;
}

// Dijkstra 算法
int *dijkstra(Graph *graph, int src, int dest)
{
    int V = graph->V;
    int *dist = malloc(V * sizeof(int));
    if (dist == NULL)
    {
        fprintf(stderr, "Unable to allocate memory for dist\n");
        exit(1);
    }

    int *parent = malloc(V * sizeof(int));
    if (parent == NULL)
    {
        fprintf(stderr, "Unable to allocate memory for parent\n");
        exit(1);
    }

    MinHeap *minHeap = createMinHeap(V);

    // 初始化最小堆
    for (int v = 0; v < V; ++v)
    {
        dist[v] = INT_MAX;
        minHeap->array[v] = newMinHeapNode(v, dist[v]);
        minHeap->pos[v] = v;
        parent[v] = -1;
    }

    // 初始化源节点
    minHeap->array[src] = newMinHeapNode(src, dist[src]);
    minHeap->pos[src] = src;
    dist[src] = 0;
    decreaseKey(minHeap, src, dist[src]);

    minHeap->size = V;

    while (!isEmpty(minHeap))
    {
        // 提取最小距离节点
        MinHeapNode *minHeapNode = extractMin(minHeap);
        int u = minHeapNode->v;

        // 更新邻接节点的距离
        AdjListNode *pCrawl = graph->array[u].head;
        while (pCrawl != NULL)
        {
            int v = pCrawl->dest;

            // 如果找到更短的路径
            if (isInMinHeap(minHeap, v) && dist[u] != INT_MAX &&
                pCrawl->weight + dist[u] < dist[v])
            {
                dist[v] = dist[u] + pCrawl->weight;
                parent[v] = u;
                decreaseKey(minHeap, v, dist[v]);
            }
            pCrawl = pCrawl->next;
        }
    }

    // 返回最短路径
    int crawl = dest;
    int num = 0;
    while (crawl != -1)
    {
        crawl = parent[crawl];
        num++;
    }
    int *path = malloc((num + 2) * sizeof(int));
    if (path == NULL)
    {
        fprintf(stderr, "Unable to allocate memory for path\n");
        exit(1);
    }
    path[1] = dist[dest];
    crawl = dest;
    for (int i = num + 1; i > 1; i--)
    {
        path[i] = crawl;
        crawl = parent[crawl];
    }
    path[0] = num;

    return path;
}
