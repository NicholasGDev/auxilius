import { ipcMain, app } from 'electron'
import type { IpcMainInvokeEvent } from 'electron'
import { spawn } from 'child_process'
import { join } from 'path'
import { existsSync, readdirSync } from 'fs'

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

function buildCliArgs(cfg: Record<string, unknown>): string[] {
  const args = ['endpoint']
  if (cfg.context)     args.push(`--context=${cfg.context}`)
  if (cfg.resource)    args.push(`--resource=${cfg.resource}`)
  if (cfg.operation)   args.push(`--operation=${cfg.operation}`)
  if (cfg.projectPath) args.push(`--project=${cfg.projectPath}`)
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
  ipcMain.handle('endpoints:list-contexts', (_event: IpcMainInvokeEvent, projectPath: string) => {
    const dir = join(projectPath, 'back', 'app', 'Contexts')
    if (!existsSync(dir)) return []
    try {
      return readdirSync(dir, { withFileTypes: true })
        .filter((d: import('fs').Dirent) => d.isDirectory())
        .map((d: import('fs').Dirent) => d.name)
    } catch { return [] }
  })

  ipcMain.handle('endpoints:preview', async (_event: IpcMainInvokeEvent, cfg: Record<string, unknown>) => {
    const args   = [...buildCliArgs(cfg), '--preview']
    const result = await runBinary(args)
    if (result.code !== 0) return { success: false, error: result.stderr || result.stdout }
    try { return { success: true, files: JSON.parse(result.stdout) } }
    catch { return { success: false, error: result.stdout || result.stderr } }
  })

  ipcMain.handle('endpoints:generate', async (_event: IpcMainInvokeEvent, cfg: Record<string, unknown>) => {
    const args   = [...buildCliArgs(cfg), '--generate']
    const result = await runBinary(args)
    if (result.code !== 0) return { success: false, error: result.stderr || result.stdout }
    try { return JSON.parse(result.stdout) }
    catch { return { success: false, error: result.stdout || result.stderr } }
  })
}

