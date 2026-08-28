import React, { useEffect, useState } from 'react'
import { HashRouter } from 'react-router-dom'
import { AppRoutes } from './routes'
import { WelcomePage } from './pages/Welcome'
import { ProjectProvider } from './contexts/ProjectContext'
import { SettingsProvider } from './contexts/SettingsContext'

const App: React.FC = () => {
  const [projectPath, setProjectPath] = useState('')
  const [ready, setReady] = useState(false)

  // Restore last project from SQLite on startup
  useEffect(() => {
    window.auxilius.db.get('projectPath')
      .then(({ value }) => { if (value) setProjectPath(value) })
      .catch(() => { /* binary unavailable, start fresh */ })
      .finally(() => setReady(true))
  }, [])

  const handleProjectSelected = (path: string) => {
    setProjectPath(path)
    window.auxilius.db.set('projectPath', path)
  }

  const handleClearProject = () => {
    setProjectPath('')
    window.auxilius.db.set('projectPath', '')
  }

  if (!ready) return null

  if (!projectPath) {
    return (
      <SettingsProvider>
        <ProjectProvider>
          <WelcomePage onProjectSelected={handleProjectSelected} />
        </ProjectProvider>
      </SettingsProvider>
    )
  }

  return (
    <SettingsProvider>
      <ProjectProvider>
        <HashRouter>
          <AppRoutes projectPath={projectPath} onClearProject={handleClearProject} />
        </HashRouter>
      </ProjectProvider>
    </SettingsProvider>
  )
}

export default App
