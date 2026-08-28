import React from 'react'

interface StatusBarProps {
  projectPath: string
  activeSection: string
}

export const StatusBar: React.FC<StatusBarProps> = ({ projectPath, activeSection }) => {
  const projectName = projectPath.split('/').filter(Boolean).pop() ?? ''
  return (
    <div className="vsc-status-bar">
      <span className="vsc-status-item vsc-status-branch">⎇ main</span>
      {projectName && (
        <span className="vsc-status-item vsc-status-path" title={projectPath}>
          📁 {projectName}
        </span>
      )}
      <span className="vsc-status-spacer" />
      <span className="vsc-status-item">{activeSection}</span>
      <span className="vsc-status-item">Auxilius v0.1.0</span>
    </div>
  )
}
