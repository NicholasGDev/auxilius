import React from 'react'
import { Outlet } from 'react-router-dom'
import { Sidebar } from '../common/Sidebar'

interface MainLayoutProps {
  projectPath: string
  onClearProject: () => void
}

export const MainLayout: React.FC<MainLayoutProps> = ({ projectPath, onClearProject }) => (
  <div className="app-layout">
    <Sidebar projectPath={projectPath} onClearProject={onClearProject} />
    <main className="main-content">
      <Outlet />
    </main>
  </div>
)
