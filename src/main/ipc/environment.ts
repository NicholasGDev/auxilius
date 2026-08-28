import { ipcMain, app } from 'electron'
import type { IpcMainInvokeEvent } from 'electron'
import { spawn } from 'child_process'
import { join } from 'path'
import { existsSync } from 'fs'
import { homedir } from 'os'

function getBinary(): string {
  if (app.isPackaged) return join(process.resourcesPath, 'scaffold_zeus')
  const devBin = join(app.getAppPath(), 'bin', 'scaffold_zeus')
  if (existsSync(devBin)) return devBin
  return join(app.getAppPath(), 'scaffold_zeus')
}

function runBinary(args: string[]): Promise<{ stdout: string; stderr: string; code: number }> {
  return new Promise((resolve) => {
    const proc = spawn(getBinary(), args)
    let stdout = '', stderr = ''
    proc.stdout.on('data', (d: Buffer) => { stdout += d.toString() })
    proc.stderr.on('data', (d: Buffer) => { stderr += d.toString() })
    proc.on('close', (code: number | null) => resolve({ stdout, stderr, code: code ?? 0 }))
    proc.on('error', (err: Error)           => resolve({ stdout, stderr: err.message, code: 1 }))
  })
}

async function runEnv(args: string[]) {
  const result = await runBinary(['env', ...args])
  try { return JSON.parse(result.stdout) }
  catch { return { success: false, error: result.stderr || result.stdout } }
}

export function registerEnvironmentHandlers(): void {
  ipcMain.handle('environment:check', async (_event: IpcMainInvokeEvent) => {
    return runEnv(['check', `--project=${join(homedir(), 'projects', 'zeus-retail-evolution')}`])
  })

  ipcMain.handle('environment:setup-wsl-root', async (_event: IpcMainInvokeEvent) => {
    return runEnv(['setup-wsl'])
  })

  ipcMain.handle('environment:setup-nvm', async (_event: IpcMainInvokeEvent) => {
    return runEnv(['setup-nvm'])
  })

  ipcMain.handle('environment:clone-project', async (_event: IpcMainInvokeEvent) => {
    const dir = join(homedir(), 'projects', 'zeus-retail-evolution')
    return runEnv(['clone-project', `--dir=${dir}`])
  })

  ipcMain.handle('environment:configure-git', async (_event: IpcMainInvokeEvent, name: string, email: string) => {
    return runEnv(['git-config', `--name=${name}`, `--email=${email}`])
  })
}
