import React, { useEffect, useState, Component, ErrorInfo, ReactNode } from 'react'
import { HashRouter } from 'react-router-dom'
import { AppRoutes } from './routes'
import { WelcomePage } from './pages/Welcome'
import { ProjectProvider } from './contexts/ProjectContext'
import { SettingsProvider } from './contexts/SettingsContext'

class ErrorBoundary extends Component<{ children: ReactNode }, { error: string | null }> {
  state = { error: null }
  static getDerivedStateFromError(e: Error) { return { error: e.message } }
  componentDidCatch(e: Error, info: ErrorInfo) { console.error('App crash:', e, info) }
  render() {
    if (this.state.error) {
      return (
        <div style={{ display:'flex', flexDirection:'column', alignItems:'center', justifyContent:'center', height:'100vh', gap:16, color:'#f87171', fontFamily:'monospace', padding:32 }}>
          <div style={{ fontSize:'2rem' }}>⚠</div>
          <div style={{ color:'#e2e8f0', fontWeight:600 }}>Algo deu errado</div>
          <pre style={{ color:'#f87171', fontSize:'0.75rem', maxWidth:480, whiteSpace:'pre-wrap', textAlign:'center' }}>{this.state.error}</pre>
          <button onClick={() => window.location.reload()} style={{ padding:'8px 20px', background:'#3b82f6', color:'#fff', border:'none', borderRadius:6, cursor:'pointer' }}>
            Recarregar
          </button>
        </div>
      )
    }
    return this.props.children
  }
}

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
      <ErrorBoundary>
        <SettingsProvider>
          <ProjectProvider>
            <WelcomePage onProjectSelected={handleProjectSelected} />
          </ProjectProvider>
        </SettingsProvider>
      </ErrorBoundary>
    )
  }

  return (
    <ErrorBoundary>
      <SettingsProvider>
        <ProjectProvider>
          <HashRouter>
            <AppRoutes projectPath={projectPath} onClearProject={handleClearProject} />
          </HashRouter>
        </ProjectProvider>
      </SettingsProvider>
    </ErrorBoundary>
  )
}

export default App
