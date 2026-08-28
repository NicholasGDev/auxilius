import { spawn, spawnSync } from 'child_process'
import { existsSync } from 'fs'
import { join } from 'path'

// Matches \\wsl$\Distro\... and \\wsl.localhost\Distro\...
const WSL_UNC_RE = /^\\\\wsl[\$.]?(?:localhost)?\\([^\\]+)(\\.*)?$/i

export function isWslPath(p: string): boolean {
  return WSL_UNC_RE.test(p)
}

/** \\wsl$\Ubuntu\root\projects → { distro: 'Ubuntu', linuxPath: '/root/projects' } */
export function parseWslPath(p: string): { distro: string; linuxPath: string } | null {
  const m = p.match(WSL_UNC_RE)
  if (!m) return null
  const distro    = m[1]
  const linuxPath = (m[2] ?? '\\').replace(/\\/g, '/')
  return { distro, linuxPath: linuxPath || '/' }
}

/** Convert a Windows UNC WSL path to its Linux equivalent */
export function toLinuxPath(p: string): string {
  const parsed = parseWslPath(p)
  return parsed ? parsed.linuxPath : p
}

/** Detect which distro a WSL path belongs to */
export function distroOf(p: string): string {
  return parseWslPath(p)?.distro ?? 'Ubuntu'
}

/** Find scaffold_zeus inside the WSL distro */
function findWslBinary(distro: string): string {
  // Try well-known install locations in order
  const candidates = [
    '/usr/local/bin/scaffold_zeus',
    `${process.env['USERPROFILE']?.replace(/\\/g, '/') ?? '/root'}/auxilius/bin/scaffold_zeus`,
    '/root/auxilius/bin/scaffold_zeus',
    '/opt/auxilius/bin/scaffold_zeus',
  ]
  for (const c of candidates) {
    const r = spawnSync('wsl.exe', ['-d', distro, '--', 'test', '-x', c])
    if (r.status === 0) return c
  }
  return '/root/auxilius/bin/scaffold_zeus'  // fallback
}

/** Resolve the binary path — WSL binary or local binary */
export function resolveBinary(projectPath: string): { cmd: string; prefix: string[] } {
  if (process.platform !== 'win32') {
    // Linux/macOS: run directly
    const bin = process.env.NODE_ENV === 'development'
      ? join(__dirname, '../../../bin/scaffold_zeus')
      : join(process.resourcesPath, 'scaffold_zeus')
    return { cmd: bin, prefix: [] }
  }

  if (isWslPath(projectPath)) {
    const distro = distroOf(projectPath)
    const bin    = findWslBinary(distro)
    // wsl.exe -d Ubuntu -- /path/to/scaffold_zeus [args...]
    return { cmd: 'wsl.exe', prefix: ['-d', distro, '--', bin] }
  }

  // Windows-native binary (scaffold_zeus.exe beside the app)
  const nativeBin = existsSync(join(process.resourcesPath, 'scaffold_zeus.exe'))
    ? join(process.resourcesPath, 'scaffold_zeus.exe')
    : join(__dirname, '../../../bin/scaffold_zeus.exe')
  return { cmd: nativeBin, prefix: [] }
}

/** Translate a project path for use as a CLI argument */
export function adaptPath(p: string): string {
  return isWslPath(p) ? toLinuxPath(p) : p
}

/** Spawn scaffold_zeus with automatic WSL routing */
export function spawnBinary(
  projectPath: string,
  args: string[]
): Promise<{ stdout: string; stderr: string; code: number }> {
  return new Promise((resolve) => {
    const { cmd, prefix } = resolveBinary(projectPath)
    // Translate any WSL UNC paths in args to Linux paths
    const translatedArgs = args.map(a => isWslPath(a) ? adaptPath(a) : a)
    const proc = spawn(cmd, [...prefix, ...translatedArgs])
    let stdout = '', stderr = ''
    proc.stdout.on('data', (d: Buffer) => { stdout += d.toString() })
    proc.stderr.on('data', (d: Buffer) => { stderr += d.toString() })
    proc.on('close',  (code: number | null) => resolve({ stdout, stderr, code: code ?? 0 }))
    proc.on('error',  (err: Error)           => resolve({ stdout, stderr: err.message, code: 1 }))
  })
}

/** List a directory — works with both local and WSL UNC paths */
export function listDirEntries(
  dirPath: string
): Promise<Array<{ name: string; path: string; isDir: boolean }>> {
  if (process.platform === 'win32' && isWslPath(dirPath)) {
    const distro    = distroOf(dirPath)
    const linuxPath = adaptPath(dirPath)
    return new Promise((resolve) => {
      const proc = spawn('wsl.exe', ['-d', distro, '--', 'ls', '-1Ap', linuxPath])
      let out = ''
      proc.stdout.on('data', (d: Buffer) => { out += d.toString() })
      proc.on('close', () => {
        const SKIP = new Set(['.git/', 'node_modules/', 'vendor/', '__pycache__/', 'dist/', 'out/'])
        const entries = out.split('\n')
          .map(l => l.trim())
          .filter(l => l && !SKIP.has(l))
          .map(l => {
            const isDir = l.endsWith('/')
            const name  = isDir ? l.slice(0, -1) : l
            // Reconstruct UNC path for children so the tree stays navigable
            const childUNC = dirPath.replace(/\\$/, '') + '\\' + name
            return { name, path: childUNC, isDir }
          })
          .sort((a, b) => {
            if (a.isDir !== b.isDir) return a.isDir ? -1 : 1
            return a.name.localeCompare(b.name)
          })
        resolve(entries)
      })
      proc.on('error', () => resolve([]))
    })
  }

  // Native fs for local paths (works for \\wsl$\... too on Windows via SMB)
  const { readdirSync } = require('fs') as typeof import('fs')
  const SKIP_DIRS = new Set(['.git', 'node_modules', 'vendor', '.DS_Store', '__pycache__', 'dist', 'out'])
  try {
    const entries = readdirSync(dirPath, { withFileTypes: true })
    return Promise.resolve(
      entries
        .filter(e => !SKIP_DIRS.has(e.name))
        .map(e => ({ name: e.name, path: `${dirPath}/${e.name}`, isDir: e.isDirectory() }))
        .sort((a, b) => {
          if (a.isDir !== b.isDir) return a.isDir ? -1 : 1
          return a.name.localeCompare(b.name)
        })
    )
  } catch {
    return Promise.resolve([])
  }
}
