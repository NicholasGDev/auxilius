import React from 'react'
import FileTree from './FileTree'

interface ExplorerPanelProps {
  projectPath: string
}

export const ExplorerPanel: React.FC<ExplorerPanelProps> = ({ projectPath }) => (
  <aside className="vsc-explorer">
    <div className="vsc-explorer-header">
      <span className="vsc-explorer-title">EXPLORADOR</span>
    </div>
    <div className="vsc-explorer-body">
      {projectPath
        ? <FileTree rootPath={projectPath} />
        : <p className="vsc-explorer-empty">Nenhum projeto aberto</p>
      }
    </div>
  </aside>
)
