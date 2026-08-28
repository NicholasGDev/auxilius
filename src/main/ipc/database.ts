import { ipcMain } from 'electron'
import { spawnBinary } from '../wsl'

// DB is always local (~/.auxilius/auxilius.db) regardless of project path
const DB_SENTINEL = '.'

function runDb(args: string[]): Promise<unknown> {
  return new Promise((resolve, reject) => {
    spawnBinary(DB_SENTINEL, ['db', ...args]).then(({ stdout, code }) => {
      if (code !== 0) { reject(new Error(stdout.trim())); return }
      try { resolve(JSON.parse(stdout.trim())) }
      catch { reject(new Error(stdout.trim())) }
    })
  })
}

export function registerDatabaseHandlers(): void {
  ipcMain.handle('db:get',  (_e, key: string)                => runDb(['get', key]))
  ipcMain.handle('db:set',  (_e, key: string, value: string) => runDb(['set', key, value]))
  ipcMain.handle('db:list', ()                               => runDb(['list']))
}
