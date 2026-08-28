import { ipcMain, app } from 'electron'
import type { IpcMainInvokeEvent } from 'electron'
import { exec } from 'child_process'
import { join } from 'path'
import { promisify } from 'util'
import { existsSync } from 'fs'
import { spawnBinary } from '../wsl'

const execAsync = promisify(exec)

function getBinaryPath(): string {
  if (app.isPackaged) return join(process.resourcesPath, 'scaffold_zeus')
  const devBin = join(app.getAppPath(), 'bin', 'scaffold_zeus')
  if (existsSync(devBin)) return devBin
  return join(app.getAppPath(), 'scaffold_zeus')
}

function getCppSrc(): string {
  if (app.isPackaged) return join(process.resourcesPath, 'cpp')
  return join(app.getAppPath(), 'cpp')
}

export function registerScaffoldHandlers(): void {
  ipcMain.handle('scaffold:get-info', (_event: IpcMainInvokeEvent) => {
    const binaryPath = getBinaryPath()
    const srcPath    = getCppSrc()
    return { binaryPath, srcPath, exists: existsSync(binaryPath) }
  })

  ipcMain.handle('scaffold:compile', async (_event: IpcMainInvokeEvent) => {
    const outPath = getBinaryPath()
    const srcDir  = getCppSrc()
    const compile =
      `g++ -std=c++20 -O2 ` +
      `${srcDir}/main.cpp ` +
      `${srcDir}/application/ScaffoldUseCase.cpp ` +
      `${srcDir}/application/EndpointUseCase.cpp ` +
      `${srcDir}/application/EnvironmentUseCase.cpp ` +
      `${srcDir}/infra/templates/PhpTemplates.cpp ` +
      `${srcDir}/infra/scaffold/BackendScaffold.cpp ` +
      `${srcDir}/infra/scaffold/FrontendScaffold.cpp ` +
      `${srcDir}/infra/scaffold/ElectronScaffold.cpp ` +
      `${srcDir}/infra/environment/Environment.cpp ` +
      `-I${srcDir} -o "${outPath}"`
    try {
      const { stdout, stderr } = await execAsync(compile)
      return { success: true, stdout, stderr, binaryPath: outPath }
    } catch (err: any) {
      return { success: false, error: err.message, stderr: err.stderr ?? '' }
    }
  })

  ipcMain.handle('scaffold:run', async (_event: IpcMainInvokeEvent, targetDir: string) => {
    const { stdout, stderr, code } = await spawnBinary(targetDir, ['scaffold', targetDir])
    return code === 0
      ? { success: true, stdout, stderr }
      : { success: false, error: stderr || stdout, stderr }
  })
}

