import React, { useState } from 'react'
import { HashRouter } from 'react-router-dom'
import { AppRoutes } from './routes'
import { WelcomePage } from './pages/Welcome'
import { ProjectProvider } from './contexts/ProjectContext'

const App: React.FC = () => {
  const [projectPath, setProjectPath] = useState('')

  if (!projectPath) {
    return (
      <ProjectProvider>
        <WelcomePage onProjectSelected={setProjectPath} />
      </ProjectProvider>
    )
  }

  return (
    <ProjectProvider>
      <HashRouter>
        <AppRoutes projectPath={projectPath} onClearProject={() => setProjectPath('')} />
      </HashRouter>
    </ProjectProvider>
  )
}

export default App
