import { ipcMain } from 'electron'
import type { IpcMainInvokeEvent } from 'electron'
import { readdirSync } from 'fs'
import { spawnBinary, adaptPath } from '../wsl'

function buildCliArgs(cfg: Record<string, unknown>): string[] {
  const args = ['endpoint']
  if (cfg.context)     args.push(`--context=${cfg.context}`)
  if (cfg.resource)    args.push(`--resource=${cfg.resource}`)
  if (cfg.operation)   args.push(`--operation=${cfg.operation}`)
  // Translate WSL UNC path to Linux path for the C++ binary
  if (cfg.projectPath) args.push(`--project=${adaptPath(cfg.projectPath as string)}`)
  if (Array.isArray(cfg.fields) && cfg.fields.length > 0) {
    const fieldStr = (cfg.fields as Array<{ name: string; phpType: string; nullable: boolean }>)
      .map(f => `${f.name}:${f.phpType}${f.nullable ? '?' : ''}`)
      .join(',')
    args.push(`--fields=${fieldStr}`)
  }
  return args
}

// Keep interface exports for renderer type compatibility
export interface FieldDef {
  name: string
  phpType: 'string' | 'int' | 'float' | 'bool'
  voType: string
  nullable: boolean
}

export interface EndpointConfig {
  projectPath: string
  context: string
  resource: string
  operation: 'criar' | 'alterar' | 'deletar' | 'consultar' | 'detalhar'
  fields: FieldDef[]
}

export interface GeneratedFile {
  relativePath: string
  absolutePath: string
  content: string
  isNew: boolean
}

export function registerEndpointHandlers(): void {
  ipcMain.handle('endpoints:list-contexts', async (_event: IpcMainInvokeEvent, projectPath: string) => {
    // For WSL paths, list via wsl.exe ls; for local paths use readdirSync
    const { listDirEntries } = await import('../wsl')
    const contextsPath = `${projectPath}/back/app/Contexts`
    try {
      const entries = await listDirEntries(contextsPath)
      return entries.filter(e => e.isDir).map(e => e.name)
    } catch { return [] }
  })

  ipcMain.handle('endpoints:preview', async (_event: IpcMainInvokeEvent, cfg: Record<string, unknown>) => {
    const args   = [...buildCliArgs(cfg), '--preview']
    const result = await spawnBinary((cfg.projectPath as string) ?? '.', args)
    if (result.code !== 0) return { success: false, error: result.stderr || result.stdout }
    try { return { success: true, files: JSON.parse(result.stdout) } }
    catch { return { success: false, error: result.stdout || result.stderr } }
  })

  ipcMain.handle('endpoints:generate', async (_event: IpcMainInvokeEvent, cfg: Record<string, unknown>) => {
    const args   = [...buildCliArgs(cfg), '--generate']
    const result = await spawnBinary((cfg.projectPath as string) ?? '.', args)
    if (result.code !== 0) return { success: false, error: result.stderr || result.stdout }
    try { return JSON.parse(result.stdout) }
    catch { return { success: false, error: result.stdout || result.stderr } }
  })
}

