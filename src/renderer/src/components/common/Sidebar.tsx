import React from 'react'
import { NavLink } from 'react-router-dom'

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
        <svg width="28" height="28" viewBox="0 0 64 64" fill="none" xmlns="http://www.w3.org/2000/svg">
          <circle cx="32" cy="32" r="32" fill="#0f172a" />
          <text x="50%" y="56%" dominantBaseline="middle" textAnchor="middle"
            fontFamily="monospace" fontSize="30" fontWeight="bold" fill="#38bdf8">
            {'{}'}
          </text>
          <ellipse cx="32" cy="10" rx="12" ry="3.5" stroke="#38bdf8" strokeWidth="1.8" fill="none" />
        </svg>
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

    <div className="sidebar-footer">v0.1.0</div>
  </aside>
)
