const { app, BrowserWindow, ipcMain } = require('electron/main')
const path = require('node:path')

// set true when develop without core
const noCoreDevelop = true

const { spawn } = require('child_process');

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
        const fs = require('fs');
        const path = require('path');
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
        ipcMain.handle('search', (startStation, endStation) => {
            return new Promise((resolve, reject) => {
                resolve('ok');
            });
        })
    } else {
        try {
            // 启动core
            const core = spawn('core.exe');
            const readline = require('readline');

            ipcMain.handle('search', (startStation, endStation) => {
                return new Promise((resolve, reject) => {
                    const rl = readline.createInterface({
                        input: core.stdout,
                        output: process.stdout
                    });

                    rl.once('line', (line) => {
                        resolve(line);
                    });

                    core.stdin.write(`${startStation} ${endStation}\n`);

                    core.stderr.once('data', (data) => {
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
