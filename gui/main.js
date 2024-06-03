const { app, BrowserWindow, ipcMain } = require('electron/main')

const fs = require('fs');
const path = require('path');

// set true when develop without core
const noCoreDevelop = false

const { spawn } = require('child_process');
const corePath = path.join(__dirname, 'core.exe');

let errorMessage = 'None'
let win

const createWindow = () => {
    win = new BrowserWindow({
        width: 800,
        height: 600,
        title: '地铁线路图',
        webPreferences: {
            preload: path.join(__dirname, 'preload.js')
        }
    })
    win.loadFile('index.html')
    win.setMenu(null)
}

app.whenReady().then(() => {
    createWindow()
    app.on('activate', () => {
        if (BrowserWindow.getAllWindows().length === 0) {
            createWindow()
        }
    })

    ipcMain.handle('setError', (error) => {
        errorMessage = error.message
        win.loadFile('error.html')
    })
    ipcMain.handle('getError', () => errorMessage)

    ipcMain.handle('listSubway', () => {
        const dataFolderPath = path.join(__dirname, 'Data');
        return new Promise((resolve, reject) => {
            fs.readdir(dataFolderPath, (err, files) => {
                if (err) {
                    reject(err);
                    return;
                }
                const jsonFiles = files.filter(file => path.extname(file) === '.json');
                let list = {};
                let promises = jsonFiles.map(file => {
                    return new Promise((resolve, reject) => {
                        fs.readFile(path.join(__dirname, 'Data', file), 'utf8', (err, data) => {
                            if (err) {
                                reject(err);
                                return;
                            }
                            var fileContent = JSON.parse(data);
                            list[fileContent.name] = [fileContent.map, file];
                            resolve();
                        });
                    });
                });
                Promise.all(promises)
                    .then(() => {
                        resolve(list);
                    })
                    .catch(err => {
                        console.error(err);
                        reject(err);
                    });
            });
        });
    })

    if (noCoreDevelop) {
        ipcMain.handle('search', (event, startStation, endStation) => {
            return new Promise((resolve, reject) => {
                resolve('ok');
            });
        })
    } else {
        try {
            // 启动core
            let core = spawn(corePath);
            const readline = require('readline');

            ipcMain.handle('readData', (event, relativePath) => {
                // 转为绝对路径
                const absolutePath = path.join(__dirname, relativePath);
                // 检查文件是否存在
                console.log(absolutePath)
                if (!fs.existsSync(absolutePath)) {
                    throw new Error(`File does not exist: ${absolutePath}`);
                }

                // 检查文件是否可读
                fs.access(absolutePath, fs.constants.R_OK, (err) => {
                    if (err) {
                        throw new Error(`File is not readable: ${absolutePath}`);
                    }
                });
                // 停止当前的core进程
                core.kill();
                // 启动新的core进程
                core = spawn(corePath);
                return new Promise((resolve, reject) => {
                    const rl = readline.createInterface({
                        input: core.stdout,
                        output: process.stdout
                    });

                    rl.on('line', (line) => {
                        const dataStr = line.trim();
                        console.log(dataStr);
                        if (dataStr === 'path:') {
                            core.stdin.write(`${absolutePath}\n`);
                        } else if (dataStr === 'Success') {
                            rl.close();
                            core.stderr.removeAllListeners('data');
                            resolve(true);
                        } else {
                            rl.close();
                            core.stderr.removeAllListeners('data');
                            reject(dataStr);
                        }
                    });

                    core.stderr.once('data', (data) => {
                        console.log(data.toString());
                        rl.close();
                        core.stderr.removeAllListeners('data');
                        reject(data.toString());
                    });
                });
            });
            ipcMain.handle('search', (event, startStation, endStation) => {
                return new Promise((resolve, reject) => {
                    const rl = readline.createInterface({
                        input: core.stdout,
                        output: process.stdout
                    });

                    rl.once('line', (line) => {
                        rl.close();
                        core.stderr.removeAllListeners('data');
                        resolve(line);
                    });

                    core.stdin.write(`${startStation} ${endStation}\n`);

                    core.stderr.once('data', (data) => {
                        rl.close();
                        core.stderr.removeAllListeners('data');
                        reject(data.toString());
                    });
                });
            });

            // 打印子进程的错误输出
            core.stderr.on('data', (data) => {
                console.error(`stderr: ${data}`);
            });

            // 当子进程退出时，打印退出码
            core.on('close', (code) => {
                console.log(`child process exited with code ${code}`);
            });

            // 处理子进程的错误
            core.on('error', (error) => {
                errorMessage = error.message;
                win.loadFile('error.html');
            });
        } catch (error) {
            errorMessage = error.message;
            win.loadFile('error.html');
        }
    }
})

app.on('window-all-closed', () => {
    if (process.platform !== 'darwin') {
        app.quit()
    }
})
