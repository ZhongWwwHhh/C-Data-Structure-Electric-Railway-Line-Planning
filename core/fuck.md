# 需求分析

## 基本需求

填入起始站、终点站，返回快捷的乘车方案  
乘车方案计算符合实际，使用环境较广泛  

## 附加需求

有向性：站点间双向距离可能不一致，如部分站双向换乘路径存在极大差别  
简单高效性：程序用时应当达到即时程度  
可拓展性：多城市？中转点？  
易用性：图形界面，~~傻瓜操作~~符合人类思维  

# 概要设计

## 程序路径

#### 纯命令行（core）

加载地铁图  
引导接受输入  
计算 Dijkstra  
返回人类可读的结果  

#### 图形化（gui）

Electron创建界面  
加载地铁图绘制界面  
调用纯命令行程序，转义图形互交与stdio操作  
对输出进行绘制  

## 模块化

#### core

- 需要对地铁图进行预处理方便输入读取（一次性操作）  
- 读取地铁图配置，转为邻接矩阵  
- Dijkstra 寻最短路径

#### gui

- Electron 图形化  

# 详细定义

#### 邻接表

需要更改，最好使用C实现，已经预知maxn，不要过量分配  
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

#### 站点index与名称

index采用：aaabbb，aaa表示线路号，bbb表示站号，至少与成都地铁现有标注方式一致  
将名称与程序处理所用index一一对应  
```json
{
    "韦家碾001": "001001",
    "升仙湖001": "001002",
    "火车北站001": "001003",
    "人民北路001": "001004",
    "文殊院001": "001005",
    "骡马市001": "001006",
    "天府广场001": "001007",
    "锦江宾馆001": "001008",
    "华西坝001": "001009",
    "省体育馆001": "001010",
    "倪家桥001": "001011",
    "桐梓林001": "001012",
    "火车南站001": "001013",
    "高新001": "001014",
    "金融城001": "001015",
    "孵化园001": "001016",
    "锦城广场001": "001017",
    "世纪城001": "001018",
    "天府三街001": "001019",
    "天府五街001": "001020",
    "华府大道001": "001021",
    "四河001": "001022",
    "广都001": "001023",
    "五根松001": "001024",
    "华阳001": "001025",
    "海昌路001": "001026",
    "广福001": "001027",
    "红石公园001": "001028",
    "麓湖001": "001029",
    "武汉路001": "001030",
    "天府公园001": "001031",
    "西博城001": "001032",
    "广州路001": "001033",
    "兴隆湖001": "001034",
    "科学城001": "001035"
}
```

#### 地铁图

json存储格式：node为节点index，guiCoord为图形界面绘制坐标，connectNode为邻接节点数组，其中distance为权重  
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

#### Dijkstra

算法直接调就好，示例：  
需要修改  
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

#### Electron

根据地铁图内nodeCoord值绘制，绑定index  
**暴露指定地铁图文件和起终点计算两个方法到沙盒**Safety，具体看 [Electron官方文档](https://www.electronjs.org/zh/docs/latest/tutorial/tutorial-prerequisites)  
具体 nodejs 与 c 互交方式参见 [nodejs与互交](https://www.cnblogs.com/dong1/p/8890892.html)，采用最末尾的方案：  
```node
3、还有一种小众化的方式

通过child_process 方式调用EXE进程来实现数据交互。

var cp= require('child_process');
//同步的方式
letout = cp.execFileSync("testdll.exe", ["arg1","arg2"],{});


//异步的方式

cp.execFile("testdll.exe", ["arg1","arg2"], {}, (error, stdout, stderr) => {
    console.log(stdout);
})
 
这种方式NODE 会接管stdout 和 stderr ,exe中把结果通过 printf 输出。
缺点：需要先生成EXE，EXE 中调用 DLL/so 并且返回结果。
优点：不需要配置 FFI,不受DLL/so 的影响
```
