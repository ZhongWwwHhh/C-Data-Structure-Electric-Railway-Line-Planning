# 地铁线路规划

## 开发环境配置

C 的就不用说了吧，纯C/C++足够了，或许加一个序列化反序列化库（方便存站点关系图）  
GUI 使用 Electron，需要 nodejs，具体看 [Electron官方文档](https://www.electronjs.org/zh/docs/latest/tutorial/tutorial-prerequisites)  
`git clone` 本项目后，推荐使用 vscode 直接打开下层文件夹 ( `/core` `/gui` ) 以便正确识别 `.vscode` 内的相关调试配置等  

## 项目结构

`/core` 文件夹内是 C 语言编写的核心程序，整个程序可以完全独立运行，使用**命令行界面直接互交**  
`/gui` 文件夹内是 Electron 图形界面，只负责图形化和图形输入输出转命令行输入输出  
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
