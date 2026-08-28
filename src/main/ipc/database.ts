import { ipcMain } from 'electron'
import { spawn } from 'child_process'
import { join } from 'path'

function binaryPath(): string {
  return join(process.env.NODE_ENV === 'development'
    ? join(__dirname, '../../../bin')
    : process.resourcesPath,
    'scaffold_zeus')
}

function runDb(args: string[]): Promise<unknown> {
  return new Promise((resolve, reject) => {
    let out = ''
    const proc = spawn(binaryPath(), ['db', ...args])
    proc.stdout.on('data', (d: Buffer) => { out += d.toString() })
    proc.stderr.on('data', (d: Buffer) => { out += d.toString() })
    proc.on('close', () => {
      try { resolve(JSON.parse(out.trim())) }
      catch { reject(new Error(out.trim())) }
    })
  })
}

export function registerDatabaseHandlers(): void {
  ipcMain.handle('db:get',  (_e, key: string)                => runDb(['get', key]))
  ipcMain.handle('db:set',  (_e, key: string, value: string) => runDb(['set', key, value]))
  ipcMain.handle('db:list', ()                               => runDb(['list']))
}
