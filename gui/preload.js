const { contextBridge, ipcRenderer } = require('electron')

contextBridge.exposeInMainWorld('apiError', {
  getError: () => ipcRenderer.invoke('getError'),
  setError: (error) => ipcRenderer.invoke('setError', error)
})

contextBridge.exposeInMainWorld('apiSearch', {
  readData: (path) => ipcRenderer.invoke('readData', path),
  search: (startStation, endStation) => ipcRenderer.invoke('search', startStation, endStation),
  listSubway: () => ipcRenderer.invoke('listSubway')
})