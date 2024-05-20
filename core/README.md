## 基础结构

本质上是使用无向图，查询无向图内的节点到节点最短路径，可以直接跳过不带路径长度的版本，使用带有路径长度的  
做合理编号，不妨参考成都地铁所采用的编号方案，如：0602 表示6号线第2站，依次排序，可做到独立性，且可以构建标号与站名之间的映射关系。  

## 结构

- 站名-编号，上述四位编号不够用，换成三位线路号+三位站编号，共六位，可以直接使用`uint32_t`存储，构成index-站名表备查  
- 由编号组成的图，采用邻接表，从邻接表查找带权重的最短路径现有算法很多，采用Dijkstra  
参见：[图算法 单源最短路径 Dijkstra算法（邻接表/邻接矩阵+优先队列STL)](https://www.cnblogs.com/dzkang2011/p/sp_dijkstra.html)
```C++
#include <iostream>
#include <cstdio>
#include <vector>
#include <queue>
using namespace std;

#define maxn 110  //最大顶点个数
int n;       //顶点个数

struct arcnode  //边结点
{
    int vertex;     //与表头结点相邻的顶点编号
    int weight;     //连接两顶点的边的权值
    arcnode * next; //指向下一相邻接点
    arcnode() {}
    arcnode(int v,int w):vertex(v),weight(w),next(NULL) {}
};

struct vernode      //顶点结点，为每一条邻接表的表头结点
{
    int vex;    //当前定点编号
    arcnode * firarc;   //与该顶点相连的第一个顶点组成的边
}Ver[maxn];

void Init()  //建立图的邻接表需要先初始化，建立顶点结点
{
    for(int i = 1; i <= n; i++)
    {
        Ver[i].vex = i;
        Ver[i].firarc = NULL;
    }
}

void Insert(int a, int b, int w)  //尾插法，插入以a为起点，b为终点，权为w的边，效率不如头插，但是可以去重边
{
    arcnode * q = new arcnode(b, w);
    if(Ver[a].firarc == NULL)
        Ver[a].firarc = q;
    else
    {
        arcnode * p = Ver[a].firarc;
        if(p->vertex == b)
        {
            if(p->weight > w)
                p->weight = w;
            return ;
        }
        while(p->next != NULL)
        {
            if(p->next->vertex == b)
            {
                if(p->next->weight > w)
                    p->next->weight = w;
                return ;
            }
            p = p->next;
        }
        p->next = q;
    }
}
void Insert2(int a, int b, int w)   //头插法，效率更高，但不能去重边
{
    arcnode * q = new arcnode(b, w);
    if(Ver[a].firarc == NULL)
        Ver[a].firarc = q;
    else
    {
        arcnode * p = Ver[a].firarc;
        q->next = p;
        Ver[a].firarc = q;
    }
}
struct node     //顶点节点，保存id和到源顶点的估算距离，优先队列需要的类型
{
    int id;     //源顶点id和估算距离
    int w;
    friend bool operator<(node a, node b)   //因要实现最小堆，按升序排列，因而需要重载运算符，重定义优先级，以小为先
    {
        return a.w > b.w;
    }
};

#define INF 0xfffff    //权值上限
int parent[maxn];   //每个顶点的父亲节点，可以用于还原最短路径树
bool visited[maxn]; //用于判断顶点是否已经在最短路径树中，或者说是否已找到最短路径
node d[maxn];      //源点到每个顶点估算距离，最后结果为源点到所有顶点的最短路。
priority_queue<node> q; //优先队列stl实现
void Dijkstra(int s)    //Dijkstra算法，传入源顶点
{
    for(int i = 1; i <= n; i++) //初始化
    {
        d[i].id = i;
        d[i].w = INF;           //估算距离置INF
        parent[i] = -1;         //每个顶点都无父亲节点
        visited[i] = false;     //都未找到最短路
    }
    d[s].w = 0;                 //源点到源点最短路权值为0
    q.push(d[s]);               //压入队列中
    while(!q.empty())           //算法的核心，队列空说明完成了操作
    {
        node cd = q.top();      //取最小估算距离顶点
        q.pop();
        int u = cd.id;
        if(visited[u])   //注意这一句的深意，避免很多不必要的操作
            continue;
        visited[u] = true;
        arcnode * p = Ver[u].firarc;
        //松弛操作
        while(p != NULL)    //找所有与他相邻的顶点，进行松弛操作，更新估算距离，压入队列。
        {
            int v = p->vertex;
            if(!visited[v] && d[v].w > d[u].w+p->weight)
            {
                d[v].w = d[u].w+p->weight;
                parent[v] = u;
                q.push(d[v]);
            }
            p = p->next;
        }
    }
}

int main()
{
    int m, a, b, c, st, ed;
    printf("请输入顶点数和边数：\n");
    scanf("%d%d", &n, &m);
    printf("请输入边以及权值（a, b, c)\n");
    Init();     //计算前必须初始化
    while(m--)
    {
        scanf("%d%d%d", &a, &b, &c);
        Insert2(a, b, c);   //无向图注意存储两条边
        Insert2(b, a, c);
    }
    printf("请输入起点和终点：\n");
    scanf("%d%d", &st, &ed);
    Dijkstra(st);
    if(d[ed].w != INF)
        printf("最短路径权值为：%d\n", d[ed].w);
    else
        printf("不存在从顶点%d到顶点%d的最短路径。\n", st, ed);
    return 0;
}
```

## 类型定义

- 邻接表：直接使用上述Dijkstra算法所使用定义，建议改回纯C实现，或者你们喜欢C++就用吧。里面的maxn可以动态分，不用占额外空间。  
```C++
#define maxn 110  //最大顶点个数
int n;       //顶点个数

struct arcnode  //边结点
{
    int vertex;     //与表头结点相邻的顶点编号
    int weight;     //连接两顶点的边的权值
    arcnode * next; //指向下一相邻接点
    arcnode() {}
    arcnode(int v,int w):vertex(v),weight(w),next(NULL) {}
};

struct vernode      //顶点结点，为每一条邻接表的表头结点
{
    int vex;    //当前定点编号
    arcnode * firarc;   //与该顶点相连的第一个顶点组成的边
}Ver[maxn];
```
- json存储格式：node为节点index，guiCoord为图形界面绘制坐标，connectNode为邻接节点数组，其中distance为权重  
```json
{
    [
        {
            node: 001001,
            guiCoord: [
                10,
                10
            ],
            connectNode: [
                {
                    // 每一站都和其前后站点相连
                    node: 001002,
                    distance: 1
                },
                {
                    // 部分站存在换乘，显然换乘时间更长（distance）
                    node: 002001,
                    distance: 10
                }
            ]
        },
        {
            node: 001002,
            guiCoord: [
                10,
                20
            ],
            connectNode: [
                {
                    node: 001001,
                    distance: 1
                },
                {
                    node: 001002,
                    distance: 1
                }
            ]
        }
    ],
    {"春熙路": 001001, "建设路": 001005} // 存放所有index与站名对应关系
}
```

## 交互

非常重要：**由于Eletron调用需要尽可能简化，最好使用传参方式分离以下两种交互，比如`--nogui`决定会打印提示性信息**  
受题目限制，不得不保留直接输入站点名称，转成index，求路径，index转回站点名称过程  
和图形化Electron结合，只需要接受首尾index，求路径，返回index串  
此处定义console args：  
```shell
--nogui -n  打印引导
--file  -f  指定地铁图json
```

## 实现

首先需要把line.txt转成index-站名表和带权重的邻接表，邻接表权重可以先统一用1填充，将两个表序列化。其中index-站名表可以直接序列化为json格式，邻接表可以把指针解析出来再序列化成json  
```
每次程序开始，读取json文件并反序列化，快速恢复预制的两个表内容  
进循环：  
    受输入（处理方法与输入输出按上述互交分立）  
    处理（抄个算法总容易吧）  
    输出  
```

## 分工

总结一下大概有几个内容：  
- json序列化，反序列化  
- 带权重邻接表查找最短路径  
- 原始line.txt转两个表（直接转成json的文件，用反序列化读进来）  
- 一些流程上的输入输出  

## 原始要求

```
9
地铁线路规划项目说明
大城市地铁错综复杂，合理规划乘车路线能帮助我们提高通行效率。
基本要求（70 分）：
1. 程序通过读文件 line.txt, 将成都地铁多个线路的站点信息读入程序，形成地铁
图的数据结构；自定义 line.txt 文件的格式。
2. 用户输入两个站点（用中文）, 输出这两个站点之间的最短路径。假设所有两
个相邻站点之间距离相等。
例如： line.txt 的部分内容如下：
1(表示 1 号线)：韦家碾,升仙湖,火车北站,人民北路,文殊院,骡马市,天府广场,锦江
宾馆,华西坝,...
7(表示 7 号线)：北站西二路，九里堤，西南交大，花照壁，茶店子，一品天下，
金沙博物馆，文化宫，东坡路，龙爪堰，武侯大道，太平园，高朋大道，神仙树，
火车南站，三瓦窑，琉璃场，四川师大，狮子山，大观，成都东客站，迎晖路，
槐树店，双店路，崔家店，理工大学，二仙桥，八里庄，府青路，驷马桥，火车
北站9
地铁线路规划项目说明
大城市地铁错综复杂，合理规划乘车路线能帮助我们提高通行效率。
基本要求（70 分）：
1. 程序通过读文件 line.txt, 将成都地铁多个线路的站点信息读入程序，形成地铁
图的数据结构；自定义 line.txt 文件的格式。
2. 用户输入两个站点（用中文）, 输出这两个站点之间的最短路径。假设所有两
个相邻站点之间距离相等。
例如： line.txt 的部分内容如下：
1(表示 1 号线)：韦家碾,升仙湖,火车北站,人民北路,文殊院,骡马市,天府广场,锦江
宾馆,华西坝,...
7(表示 7 号线)：北站西二路，九里堤，西南交大，花照壁，茶店子，一品天下，
金沙博物馆，文化宫，东坡路，龙爪堰，武侯大道，太平园，高朋大道，神仙树，
火车南站，三瓦窑，琉璃场，四川师大，狮子山，大观，成都东客站，迎晖路，
槐树店，双店路，崔家店，理工大学，二仙桥，八里庄，府青路，驷马桥，火车
北站,北站西二路（和第一个站点重，表示环线）
用户输入：升仙湖 西南交大
程序输出：1 号线 升仙湖 火车北站，转 7 号线 北站西二路 九里堤 西南交
大
3. 用户输入两个站点（用中文）, 设置规划条件，例如转车次数少、花钱少等，
输出规划乘车线路。
扩展功能：（30 分，每增加一个功能加 8~10 分）
1. 使用图形化界面，用户可以看到地铁图。计算出路径后，可以在图上高亮显
示。
2. 站点之间距离按实际距离计算，并考虑换乘距离，按最短距离规划乘车线路；
3. 输入文件考虑各线路发车间隔、以及不同时段发车间隔，按最短时间距离规
划乘车线路。
4. 其他符合实际情况的规划需求等
```