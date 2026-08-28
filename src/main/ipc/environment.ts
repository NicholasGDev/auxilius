import { ipcMain } from 'electron'
import type { IpcMainInvokeEvent } from 'electron'
import { join } from 'path'
import { homedir } from 'os'
import { readFileSync, writeFileSync, mkdirSync } from 'fs'
import { spawn } from 'child_process'
import { spawnBinary } from '../wsl'

// scaffold_zeus's env checks (git, nvm, /etc/wsl.conf) only make sense inside
// the Linux/WSL filesystem, so the default project path must be a Linux path
// even when Electron itself is running natively on Windows.
const DEFAULT_PROJECT_PATH = process.platform === 'win32'
  ? '/root/projects/zeus-retail-evolution'
  : join(homedir(), 'projects', 'zeus-retail-evolution')

async function runEnv(args: string[]) {
  const result = await spawnBinary(DEFAULT_PROJECT_PATH, ['env', ...args])
  try { return JSON.parse(result.stdout) }
  catch { return { success: false, error: result.stderr || result.stdout } }
}

export function registerEnvironmentHandlers(): void {
  ipcMain.handle('environment:check', async (_event: IpcMainInvokeEvent) => {
    return runEnv(['check', `--project=${DEFAULT_PROJECT_PATH}`])
  })

  ipcMain.handle('environment:setup-wsl-root', async (_event: IpcMainInvokeEvent) => {
    return runEnv(['setup-wsl'])
  })

  ipcMain.handle('environment:setup-nvm', async (_event: IpcMainInvokeEvent) => {
    return runEnv(['setup-nvm'])
  })

  ipcMain.handle('environment:clone-project', async (_event: IpcMainInvokeEvent) => {
    return runEnv(['clone-project', `--dir=${DEFAULT_PROJECT_PATH}`])
  })

  ipcMain.handle('environment:configure-git', async (_event: IpcMainInvokeEvent, name: string, email: string) => {
    return runEnv(['git-config', `--name=${name}`, `--email=${email}`])
  })

  // ── Onboarding wizard handlers ───────────────────────────────────────────────

  // Check if WSL Ubuntu is reachable (Windows: ping echo; Linux: always true)
  ipcMain.handle('environment:wsl-check', async () => {
    if (process.platform !== 'win32') {
      return { isLinux: true, wslAvailable: true, ubuntuAvailable: true }
    }
    return new Promise((resolve) => {
      const proc = spawn('wsl.exe', ['-d', 'Ubuntu', '--', 'echo', 'ok'])
      let out = ''
      proc.stdout.on('data', (d: Buffer) => { out += d.toString() })
      proc.on('close', (code) => {
        const ubuntuAvailable = code === 0 && out.trim() === 'ok'
        resolve({ isLinux: false, wslAvailable: code !== 127, ubuntuAvailable })
      })
      proc.on('error', () => resolve({ isLinux: false, wslAvailable: false, ubuntuAvailable: false }))
    })
  })

  // Write [wsl2]\nmemory=6GB to %USERPROFILE%\.wslconfig (Windows only)
  ipcMain.handle('environment:wsl-memory', async () => {
    if (process.platform !== 'win32') {
      return { success: true, skipped: true, message: 'N\u00e3o aplic\u00e1vel no Linux' }
    }
    const wslconfigPath = join(homedir(), '.wslconfig')
    let content = ''
    try { content = readFileSync(wslconfigPath, 'utf8') } catch {}
    if (/\[wsl2\]/i.test(content)) {
      content = /^memory\s*=/im.test(content)
        ? content.replace(/^memory\s*=.*/im, 'memory=6GB')
        : content.replace(/(\[wsl2\])/i, '$1\nmemory=6GB')
    } else {
      content = (content.trim() ? content.trim() + '\n\n' : '') + '[wsl2]\nmemory=6GB\n'
    }
    try {
      writeFileSync(wslconfigPath, content)
      spawn('wsl.exe', ['--shutdown'])
      return { success: true, message: `Gravado: ${wslconfigPath} — WSL reiniciado` }
    } catch (e: any) {
      return { success: false, error: e.message }
    }
  })

  // Create ~/projects directory (inside WSL on Windows, native fs on Linux)
  ipcMain.handle('environment:mkdir-projects', async () => {
    const projectsDir = process.platform === 'win32'
      ? '/root/projects'
      : join(homedir(), 'projects')
    if (process.platform !== 'win32') {
      try {
        mkdirSync(projectsDir, { recursive: true })
        return { success: true, message: `Criado: ${projectsDir}` }
      } catch (e: any) {
        return { success: false, error: e.message }
      }
    }
    return new Promise<{ success: boolean; message?: string; error?: string }>((resolve) => {
      const proc = spawn('wsl.exe', ['-d', 'Ubuntu', '--', 'mkdir', '-p', projectsDir])
      let stderr = ''
      proc.stderr?.on('data', (d: Buffer) => { stderr += d.toString() })
      proc.on('close', (code) => {
        resolve(code === 0
          ? { success: true, message: `Criado: ${projectsDir}` }
          : { success: false, error: stderr || 'Erro ao criar diret\u00f3rio' })
      })
      proc.on('error', (e: Error) => resolve({ success: false, error: e.message }))
    })
  })
}
