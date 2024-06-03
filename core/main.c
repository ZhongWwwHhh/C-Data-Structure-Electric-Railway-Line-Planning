#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "cJSON.h"
#include "graph2.h"

char *getLineName(const cJSON *jsonLines, const int *sitesIndex, const int dijIndex)
{
    // 线路和站点index转为字符型
    char lineKey[4];
    sprintf(lineKey, "%03d", sitesIndex[dijIndex] / 1000);
    cJSON *jsonLine = cJSON_GetObjectItemCaseSensitive(jsonLines, lineKey);
    if (jsonLine == NULL)
    {
        fprintf(stderr, "Unable to get line with key %s\n", lineKey);
        exit(1);
    }

    char *jsonLineValue = cJSON_GetStringValue(jsonLine);
    if (jsonLineValue == NULL)
    {
        fprintf(stderr, "Unable to get string value of line\n");
        exit(1);
    }

    return jsonLineValue;
}
char *getSiteName(const cJSON *jsonSites, const int *sitesIndex, const int dijIndex)
{
    // 线路和站点index转为字符型
    char siteKey[7];
    sprintf(siteKey, "%06d", sitesIndex[dijIndex]);
    cJSON *jsonSite = cJSON_GetObjectItemCaseSensitive(jsonSites, siteKey);
    if (jsonSite == NULL)
    {
        fprintf(stderr, "Unable to get site with key %s\n", siteKey);
        exit(1);
    }

    char *jsonSiteValue = cJSON_GetStringValue(jsonSite);
    if (jsonSiteValue == NULL)
    {
        fprintf(stderr, "Unable to get string value of site\n");
        exit(1);
    }

    return jsonSiteValue;
}

int main()
{
    // 从stdin获取JSON文件地址
    char jsonFilePath[256];
    printf("path:\n");
    fflush(stdout);
    fgets(jsonFilePath, sizeof(jsonFilePath), stdin);
    jsonFilePath[strcspn(jsonFilePath, "\n")] = '\0';

    // 打开并读取JSON文件
    FILE *fp = fopen(jsonFilePath, "r");
    if (fp == NULL)
    {
        perror("Unable to open file!");
        exit(1);
    }

    // 获取文件大小
    fseek(fp, 0, SEEK_END);
    long fsize = ftell(fp);

    // 根据文件大小进行malloc
    char *content = malloc(fsize + 1);
    if (content == NULL)
    {
        perror("Unable to allocate memory!");
        exit(1);
    }
    content[0] = '\0';

    // 读取
    fseek(fp, 0, SEEK_SET);
    char chunk[256];
    while (fgets(chunk, sizeof(chunk), fp) != NULL)
    {
        content = realloc(content, strlen(content) + strlen(chunk) + 1);
        strcat(content, chunk);
    }

    fclose(fp);

    // 解析JSON内容
    cJSON *json = cJSON_Parse(content);
    free(content);
    if (json == NULL)
    {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL)
        {
            fprintf(stderr, "Error before: %s\n", error_ptr);
        }
        exit(1);
    }

    // index-站名
    cJSON *jsonSites = cJSON_GetObjectItem(json, "sites");
    // line-线路名
    cJSON *jsonLines = cJSON_GetObjectItem(json, "lines");
    if (jsonSites == NULL)
    {
        fprintf(stderr, "Unable to find 'sites' object in JSON\n");
        exit(1);
    }
    if (jsonLines == NULL)
    {
        fprintf(stderr, "Unable to find 'lines' object in JSON\n");
        exit(1);
    }

    // 邻接表index-index
    int numSites = 0;
    for (cJSON *site = jsonSites->child; site != NULL; site = site->next)
    {
        numSites++;
    }
    int *sitesIndex = malloc(numSites * sizeof(int));
    if (sitesIndex == NULL)
    {
        perror("Unable to allocate memory!");
        exit(1);
    }
    int i = 0;
    for (cJSON *site = jsonSites->child; site != NULL; site = site->next)
    {
        const char *siteNumberStr = site->string;
        int siteNumber = atoi(siteNumberStr);
        sitesIndex[i] = siteNumber;
        i++;
    }

    // 创建邻接表
    cJSON *table = cJSON_GetObjectItem(json, "table");
    if (numSites != cJSON_GetArraySize(table))
    {
        fprintf(stderr, "Number of sites does not match the number of entries in the table\n");
        exit(1);
    }
    Graph *subwayGraph = createGraph(numSites);
    for (int i = 0; i < numSites; i++)
    {
        cJSON *node = cJSON_GetArrayItem(table, i);
        cJSON *connectNode = cJSON_GetObjectItem(node, "connectNode");
        int numConnectNodes = cJSON_GetArraySize(connectNode);
        for (int j = 0; j < numConnectNodes; j++)
        {
            cJSON *adjNode = cJSON_GetArrayItem(connectNode, j);
            cJSON *dest = cJSON_GetObjectItem(adjNode, "node");
            cJSON *weight = cJSON_GetObjectItem(adjNode, "distance");

            const char *destStr = cJSON_GetStringValue(dest);
            if (destStr == NULL)
            {
                fprintf(stderr, "Unable to get string value of 'node'\n");
                exit(1);
            }

            int destValue = atoi(destStr);

            int destIndex = -1;
            for (int k = 0; k < numSites; k++)
            {
                if (sitesIndex[k] == destValue)
                {
                    destIndex = k;
                    break;
                }
            }

            if (destIndex == -1)
            {
                fprintf(stderr, "Unable to find site number in sitesIndex\n");
                exit(1);
            }

            insertEdge(subwayGraph, i, destIndex, weight->valueint);
        }
    }

    // printGraph(subwayGraph);

    // 读取文件成功
    puts("Success");
    fflush(stdout);

    int srcIndex, destIndex;

    while (1)
    {
        scanf("%d %d", &srcIndex, &destIndex);

        int dest = -1;
        int src = -1;
        for (int k = 0; k < numSites; k++)
        {
            if (sitesIndex[k] == destIndex)
            {
                dest = k;
            }
            if (sitesIndex[k] == srcIndex)
            {
                src = k;
            }
        }

        if (dest == -1 || src == -1)
        {
            fprintf(stderr, "Unable to find site number in sitesIndex\n");
            exit(1);
        }

        int *path = dijkstra(subwayGraph, src, dest);

        if (path[0] == -1)
        {
            printf("无法到达\n");
            fflush(stdout);
            continue;
        }
        if (path[0] == 1)
        {
            printf("起点终点相同\n");
            fflush(stdout);
            continue;
        }

        printf("用时: %d 分钟<br>", path[1]);

        char siteKey[7];
        char lineKey[4];
        int lineNow = -1;
        for (int i = 2; i < path[0] + 2; i++)
        {
            if (i == 2)
            {
                if (sitesIndex[path[i]] / 1000 != sitesIndex[path[i + 1]] / 1000)
                {
                    lineNow = sitesIndex[path[i + 1]] / 1000;
                    char *jsonSiteValue = getSiteName(jsonSites, sitesIndex, path[i]);
                    char *jsonLineValue = getLineName(jsonLines, sitesIndex, path[i]);
                    char *jsonLineValueOtherStart = getLineName(jsonLines, sitesIndex, path[i + 1]);
                    printf("%s/%s: %s -> ", jsonLineValue, jsonLineValueOtherStart, jsonSiteValue);
                    continue;
                }
            }
            if (i == path[0])
            {
                if (sitesIndex[path[i]] / 1000 != sitesIndex[path[i + 1]] / 1000)
                {
                    lineNow = sitesIndex[path[i + 1]] / 1000;
                    char *jsonSiteValue = getSiteName(jsonSites, sitesIndex, path[i]);
                    char *jsonLineValue = getLineName(jsonLines, sitesIndex, path[i]);
                    char *jsonLineValueOtherEnd = getLineName(jsonLines, sitesIndex, path[i + 1]);
                    printf("%s/%s: %s", jsonLineValue, jsonLineValueOtherEnd, jsonSiteValue);
                    break;
                }
            }
            if (lineNow != sitesIndex[path[i]] / 1000 || i == path[0] + 1)
            {
                lineNow = sitesIndex[path[i]] / 1000;

                for (int j = (i == path[0] + 1 || i == 2 ? i : i - 1); j <= i; j++)
                {
                    char *jsonSiteValue = getSiteName(jsonSites, sitesIndex, path[j]);
                    char *jsonLineValue = getLineName(jsonLines, sitesIndex, path[j]);

                    printf("%s: %s", jsonLineValue, jsonSiteValue);
                    if (j != i)
                    {
                        printf("<br>");
                    }
                    else if (i != path[0] + 1)
                    {
                        printf(" -> ");
                    }
                }
            }
        }
        printf("\n");
        fflush(stdout);
    }

    // 释放
    cJSON_Delete(json);
    free(sitesIndex);
    free(subwayGraph);

    exit(0);
}
