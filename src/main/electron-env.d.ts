/// <reference types="electron" />

// Augment NodeJS.Process with Electron-specific fields available in the main process
declare namespace NodeJS {
  interface Process {
    resourcesPath: string
  }
}
