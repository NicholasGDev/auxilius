import React, { createContext, useContext, useState } from 'react'

interface ProjectContextValue {
  projectPath: string
  setProjectPath: (path: string) => void
}

const ProjectContext = createContext<ProjectContextValue>({
  projectPath: '',
  setProjectPath: () => undefined,
})

export const ProjectProvider: React.FC<{ children: React.ReactNode }> = ({ children }) => {
  const [projectPath, setProjectPath] = useState('')
  return (
    <ProjectContext.Provider value={{ projectPath, setProjectPath }}>
      {children}
    </ProjectContext.Provider>
  )
}

export const useProject = () => useContext(ProjectContext)
