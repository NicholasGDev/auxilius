import type { FieldDef, EndpointConfig, GeneratedFile } from '../../main/ipc/endpoints'

interface ScaffoldInfo {
  srcPath: string
  exists: boolean
}

interface CommandResult {
  success: boolean
  stdout?: string
  stderr?: string
  error?: string
  binaryPath?: string
  message?: string
  written?: string[]
  skipped?: string[]
}

interface CheckResult {
  ok: boolean
  version?: string
  message?: string
}

interface EnvironmentStatus {
  git: CheckResult
  nvm: CheckResult
  node20: CheckResult
  node19: CheckResult
  rootDefault: boolean
  projectExists: boolean
  projectPath: string
}

interface PreviewResult {
  success: boolean
  files?: GeneratedFile[]
  error?: string
}

interface GenerateResult {
  success: boolean
  written?: string[]
  skipped?: string[]
  error?: string
}

declare global {
  interface Window {
    auxilius: {
      scaffold: {
        getInfo():                        Promise<ScaffoldInfo>
        compile():                        Promise<CommandResult>
        run(targetDir: string):           Promise<CommandResult>
      }
      dialog: {
        openDirectory():                  Promise<string | null>
      }
      environment: {
        check():                                    Promise<EnvironmentStatus>
        setupWslRoot():                             Promise<CommandResult>
        setupNvm():                                 Promise<CommandResult>
        cloneProject():                             Promise<CommandResult>
        configureGit(name: string, email: string):  Promise<CommandResult>
      }
      endpoints: {
        listContexts(projectPath: string):  Promise<string[]>
        preview(cfg: EndpointConfig):       Promise<PreviewResult>
        generate(cfg: EndpointConfig):      Promise<GenerateResult>
      }
    }
  }
}

export type { FieldDef, EndpointConfig, GeneratedFile, CommandResult, EnvironmentStatus, CheckResult }
