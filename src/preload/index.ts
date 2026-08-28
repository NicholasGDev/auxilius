import { contextBridge, ipcRenderer } from 'electron'

const auxilius = {
  scaffold: {
    getInfo:  ()                    => ipcRenderer.invoke('scaffold:get-info'),
    compile:  ()                    => ipcRenderer.invoke('scaffold:compile'),
    run:      (targetDir: string)   => ipcRenderer.invoke('scaffold:run', targetDir),
  },
  dialog: {
    openDirectory: (): Promise<string | null> => ipcRenderer.invoke('dialog:open-directory'),
  },
  fs: {
    listDir: (dirPath: string): Promise<Array<{ name: string; path: string; isDir: boolean }>> =>
      ipcRenderer.invoke('fs:list-dir', dirPath),
  },
  environment: {
    check:          ()                             => ipcRenderer.invoke('environment:check'),
    setupWslRoot:   ()                             => ipcRenderer.invoke('environment:setup-wsl-root'),
    setupNvm:       ()                             => ipcRenderer.invoke('environment:setup-nvm'),
    cloneProject:   ()                             => ipcRenderer.invoke('environment:clone-project'),
    configureGit:   (name: string, email: string)  => ipcRenderer.invoke('environment:configure-git', name, email),
  },
  endpoints: {
    listContexts: (projectPath: string) => ipcRenderer.invoke('endpoints:list-contexts', projectPath),
    preview:      (cfg: unknown)        => ipcRenderer.invoke('endpoints:preview', cfg),
    generate:     (cfg: unknown)        => ipcRenderer.invoke('endpoints:generate', cfg),
  },
  db: {
    get:  (key: string)                => ipcRenderer.invoke('db:get',  key)                as Promise<{key:string;value:string}>,
    set:  (key: string, value: string) => ipcRenderer.invoke('db:set',  key, value)         as Promise<{ok:boolean}>,
    list: ()                           => ipcRenderer.invoke('db:list')                     as Promise<Array<{key:string;value:string}>>,
  },
}

contextBridge.exposeInMainWorld('auxilius', auxilius)
