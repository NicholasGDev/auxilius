import React, { createContext, useContext, useEffect, useState } from 'react'

interface Settings {
  projectPath: string
  lastContext: string
  lastResource: string
  lastOperation: string
  lastFields: string
  gitName: string
  gitEmail: string
}

const DEFAULTS: Settings = {
  projectPath:   '',
  lastContext:   '',
  lastResource:  '',
  lastOperation: 'consultar',
  lastFields:    '',
  gitName:       '',
  gitEmail:      '',
}

interface SettingsContextValue {
  settings: Settings
  setSetting: <K extends keyof Settings>(key: K, value: Settings[K]) => void
  loaded: boolean
}

const SettingsContext = createContext<SettingsContextValue>({
  settings:   DEFAULTS,
  setSetting: () => {},
  loaded:     false,
})

export const SettingsProvider: React.FC<{ children: React.ReactNode }> = ({ children }) => {
  const [settings, setSettings] = useState<Settings>(DEFAULTS)
  const [loaded, setLoaded]     = useState(false)

  useEffect(() => {
    window.auxilius.db.list().then((rows) => {
      const map: Partial<Settings> = {}
      for (const { key, value } of rows)
        if (key in DEFAULTS) (map as Record<string, string>)[key] = value
      setSettings(s => ({ ...s, ...map }))
      setLoaded(true)
    })
  }, [])

  const setSetting = <K extends keyof Settings>(key: K, value: Settings[K]) => {
    setSettings(s => ({ ...s, [key]: value }))
    window.auxilius.db.set(key, String(value))
  }

  return (
    <SettingsContext.Provider value={{ settings, setSetting, loaded }}>
      {children}
    </SettingsContext.Provider>
  )
}

export const useSettings = () => useContext(SettingsContext)
