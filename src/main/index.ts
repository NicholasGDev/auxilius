import { app, BrowserWindow, ipcMain, dialog } from 'electron'
import { join } from 'path'
import { registerScaffoldHandlers } from './ipc/scaffold'
import { registerEnvironmentHandlers } from './ipc/environment'
import { registerEndpointHandlers } from './ipc/endpoints'
import { registerDatabaseHandlers } from './ipc/database'
import { listDirEntries } from './wsl'

const isDev = process.env.NODE_ENV === 'development'

// prevents black screen on Windows when GPU acceleration is unavailable
if (!isDev) app.disableHardwareAcceleration()

function createWindow(): void {
  const mainWindow = new BrowserWindow({
    width: 1280,
    height: 800,
    minWidth: 1024,
    minHeight: 700,
    show: false,
    autoHideMenuBar: true,
    title: 'Auxilius — Zeus Retail Evolution',
    webPreferences: {
      preload: join(__dirname, '../preload/index.js'),
      sandbox: false,
      contextIsolation: true,
      nodeIntegration: false
    }
  })

  mainWindow.on('ready-to-show', () => mainWindow.show())

  mainWindow.webContents.on('did-fail-load', (_e, code, desc) => {
    console.error('Renderer failed to load:', code, desc)
    mainWindow.webContents.openDevTools()
  })

  if (!isDev) mainWindow.webContents.openDevTools()

  if (isDev && process.env['ELECTRON_RENDERER_URL']) {
    mainWindow.loadURL(process.env['ELECTRON_RENDERER_URL'])
  } else {
    const rendererPath = join(__dirname, '../renderer/index.html')
    mainWindow.loadFile(rendererPath).catch(() => {
      mainWindow.webContents.openDevTools()
    })
  }

  // open DevTools on renderer crash to expose the actual error
  mainWindow.webContents.on('did-fail-load', (_e, code, desc) => {
    console.error('Renderer failed to load:', code, desc)
    mainWindow.webContents.openDevTools()
  })
}

app.whenReady().then(() => {
  registerScaffoldHandlers()
  registerEnvironmentHandlers()
  registerEndpointHandlers()
  registerDatabaseHandlers()

  ipcMain.handle('dialog:open-directory', async () => {
    const win = BrowserWindow.getFocusedWindow()
    const result = await dialog.showOpenDialog(win!, {
      properties: ['openDirectory', 'createDirectory'],
      title: 'Selecionar diretório do projeto'
    })
    return result.canceled ? null : result.filePaths[0]
  })

  ipcMain.handle('fs:list-dir', async (_event, dirPath: string) => {
    return listDirEntries(dirPath)
  })

  createWindow()

  app.on('activate', () => {
    if (BrowserWindow.getAllWindows().length === 0) createWindow()
  })
})

app.on('window-all-closed', () => {
  if (process.platform !== 'darwin') app.quit()
})
