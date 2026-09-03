import React from 'react'
import { NavLink } from 'react-router-dom'
import logo from '../../assets/logo.png'

const NAV = [
  { to: '/home',      icon: '⬡', label: 'Dashboard' },
  { to: '/setup',     icon: '⚙', label: 'Ambiente' },
  { to: '/scaffold',  icon: '◈', label: 'Scaffold' },
  { to: '/endpoints', icon: '⊕', label: 'Endpoints' },
]

interface SidebarProps {
  projectPath: string
  onClearProject: () => void
}

export const Sidebar: React.FC<SidebarProps> = ({ projectPath, onClearProject }) => (
  <aside className="sidebar">
    <div className="sidebar-brand">
      <div className="sidebar-logo">
        <img src={logo} width="28" height="28" alt="Auxilius" style={{ borderRadius: 6 }} />
      </div>
      <div>
        <div className="brand-title">Auxilius</div>
        <div className="brand-sub">Zeus Retail Evolution</div>
      </div>
    </div>

    <nav className="sidebar-nav">
      {NAV.map(item => (
        <NavLink
          key={item.to}
          to={item.to}
          className={({ isActive }) => `nav-item${isActive ? ' active' : ''}`}
        >
          <span className="nav-icon">{item.icon}</span>
          {item.label}
        </NavLink>
      ))}
    </nav>

    <div className="sidebar-project">
      <div className="project-label">Projeto ativo</div>
      <div className="project-path" title={projectPath}>{projectPath}</div>
      <button className="project-change" onClick={onClearProject}>Trocar projeto</button>
    </div>

    <div className="sidebar-footer">v0.1.2</div>
  </aside>
)
