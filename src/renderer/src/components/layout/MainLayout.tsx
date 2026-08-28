import React, { useState } from 'react'
import { Outlet, NavLink, useLocation, useNavigate } from 'react-router-dom'
import { ActivityBar, ActivitySection } from './ActivityBar'
import { StatusBar } from './StatusBar'
import FileTree from './FileTree'

const SECTION_LABELS: Record<ActivitySection, string> = {
  explorer:  'EXPLORADOR',
  scaffold:  'SCAFFOLD',
  endpoints: 'ENDPOINTS',
  setup:     'AMBIENTE',
  welcome:   '',
}

const NAV_ITEMS = [
  { to: '/home',      section: 'setup' as ActivitySection,     label: 'Dashboard' },
  { to: '/setup',     section: 'setup' as ActivitySection,     label: 'Ambiente' },
  { to: '/scaffold',  section: 'scaffold' as ActivitySection,  label: 'Scaffold' },
  { to: '/endpoints', section: 'endpoints' as ActivitySection, label: 'Endpoints' },
]

function pathToSection(pathname: string): ActivitySection {
  if (pathname.startsWith('/scaffold'))  return 'scaffold'
  if (pathname.startsWith('/endpoints')) return 'endpoints'
  if (pathname.startsWith('/setup'))     return 'setup'
  return 'explorer'
}

interface MainLayoutProps {
  projectPath: string
  onClearProject: () => void
}

export const MainLayout: React.FC<MainLayoutProps> = ({ projectPath, onClearProject }) => {
  const location  = useLocation()
  const navigate  = useNavigate()
  const [active, setActive] = useState<ActivitySection>(pathToSection(location.pathname))
  const [sidebarOpen, setSidebarOpen] = useState(true)

  const handleActivityClick = (section: ActivitySection) => {
    if (active === section) {
      setSidebarOpen(o => !o)
    } else {
      setActive(section)
      setSidebarOpen(true)
      if (section === 'scaffold')  navigate('/scaffold')
      if (section === 'endpoints') navigate('/endpoints')
      if (section === 'setup')     navigate('/setup')
      if (section === 'explorer')  navigate('/home')
    }
  }

  return (
    <div className={`vsc-shell${sidebarOpen ? '' : ' sidebar-collapsed'}`}>

      <ActivityBar
        active={active}
        onChange={handleActivityClick}
        onSwitch={onClearProject}
      />

      <aside className="vsc-sidebar">
        <div className="sidebar-title">
          {SECTION_LABELS[active]}
          <button onClick={() => setSidebarOpen(false)} title="Fechar painel">✕</button>
        </div>
        <div className="sidebar-scroll">
          {active === 'explorer' && (
            projectPath
              ? <FileTree rootPath={projectPath} />
              : <p style={{ padding: '12px', fontSize: '0.8rem', color: '#858585' }}>Nenhum projeto aberto</p>
          )}
          {(active === 'scaffold' || active === 'endpoints' || active === 'setup') && (
            <nav className="vsc-nav">
              <div className="vsc-nav-group">Navegação</div>
              {NAV_ITEMS.map(item => (
                <NavLink
                  key={item.to}
                  to={item.to}
                  className={({ isActive }) => `vsc-nav-item${isActive ? ' active' : ''}`}
                >
                  <span className="vsc-nav-icon">›</span>
                  {item.label}
                </NavLink>
              ))}
              {projectPath && (
                <>
                  <div className="vsc-nav-group" style={{ marginTop: 8 }}>Projeto</div>
                  <div style={{ padding: '4px 12px', fontSize: '0.75rem', color: '#9cdcfe', fontFamily: 'monospace', wordBreak: 'break-all' }}>
                    {projectPath}
                  </div>
                </>
              )}
            </nav>
          )}
        </div>
      </aside>

      <main className="vsc-editor">
        <div className="editor-tabs">
          {NAV_ITEMS.map(item => (
            <NavLink
              key={item.to}
              to={item.to}
              className={({ isActive }) => `editor-tab${isActive ? ' active' : ''}`}
            >
              <span className="tab-icon">
                {item.to === '/home' && '⬡'}
                {item.to === '/setup' && '⚙'}
                {item.to === '/scaffold' && '◈'}
                {item.to === '/endpoints' && '⊕'}
              </span>
              {item.label}
            </NavLink>
          ))}
        </div>
        <div className="editor-content">
          <Outlet />
        </div>
      </main>

      <StatusBar projectPath={projectPath} activeSection={SECTION_LABELS[active] || 'Explorer'} />
    </div>
  )
}

