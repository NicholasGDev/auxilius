import React from 'react'

interface StatusBarProps {
  projectPath: string
}

export const StatusBar: React.FC<StatusBarProps> = ({ projectPath }) => {
  const projectName = projectPath.split('/').filter(Boolean).pop() ?? ''
  return (
    <div className="vsc-status-bar">
      <span className="vsc-status-item vsc-status-branch">⎇ main</span>
      <span className="vsc-status-item vsc-status-path" title={projectPath}>
        {projectName && `📁 ${projectName}`}
      </span>
      <span className="vsc-status-spacer" />
      <span className="vsc-status-item">Auxilius v0.1.0</span>
      <span className="vsc-status-item">Zeus Retail Evolution</span>
    </div>
  )
}
