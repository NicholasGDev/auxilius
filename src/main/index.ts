import { app, BrowserWindow, ipcMain, dialog } from 'electron'
import { join } from 'path'
import { readdirSync } from 'fs'
import { registerScaffoldHandlers } from './ipc/scaffold'
import { registerEnvironmentHandlers } from './ipc/environment'
import { registerEndpointHandlers } from './ipc/endpoints'

const isDev = process.env.NODE_ENV === 'development'

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

  if (isDev && process.env['ELECTRON_RENDERER_URL']) {
    mainWindow.loadURL(process.env['ELECTRON_RENDERER_URL'])
  } else {
    mainWindow.loadFile(join(__dirname, '../renderer/index.html'))
  }
}

app.whenReady().then(() => {
  registerScaffoldHandlers()
  registerEnvironmentHandlers()
  registerEndpointHandlers()

  ipcMain.handle('dialog:open-directory', async () => {
    const win = BrowserWindow.getFocusedWindow()
    const result = await dialog.showOpenDialog(win!, {
      properties: ['openDirectory', 'createDirectory'],
      title: 'Selecionar diretório do projeto'
    })
    return result.canceled ? null : result.filePaths[0]
  })

  const SKIP_DIRS = new Set(['.git', 'node_modules', 'vendor', '.DS_Store', '__pycache__', 'dist', 'out'])
  ipcMain.handle('fs:list-dir', async (_event, dirPath: string) => {
    try {
      const entries = readdirSync(dirPath, { withFileTypes: true })
      return entries
        .filter(e => !SKIP_DIRS.has(e.name))
        .map(e => ({ name: e.name, path: `${dirPath}/${e.name}`, isDir: e.isDirectory() }))
        .sort((a, b) => {
          if (a.isDir !== b.isDir) return a.isDir ? -1 : 1
          return a.name.localeCompare(b.name)
        })
    } catch {
      return []
    }
  })

  createWindow()

  app.on('activate', () => {
    if (BrowserWindow.getAllWindows().length === 0) createWindow()
  })
})

app.on('window-all-closed', () => {
  if (process.platform !== 'darwin') app.quit()
})
