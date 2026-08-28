import React, { useCallback, useEffect, useState } from 'react'

interface TreeNode {
  name: string
  path: string
  isDir: boolean
}

interface NodeState {
  expanded: boolean
  children: TreeNode[] | null  // null = not yet loaded
}

interface FileTreeProps {
  rootPath: string
}

// Icon by file extension
function fileIcon(name: string, isDir: boolean, expanded: boolean): string {
  if (isDir) return expanded ? '📂' : '📁'
  const ext = name.split('.').pop()?.toLowerCase() ?? ''
  const map: Record<string, string> = {
    php: '🐘', ts: '📘', tsx: '📘', js: '📙', jsx: '📙',
    scss: '🎨', css: '🎨', json: '📋', md: '📝', sh: '⚙',
    html: '🌐', xml: '📄', env: '🔒', gitignore: '🚫',
  }
  return map[ext] ?? '📄'
}

function fileColor(name: string, isDir: boolean): string {
  if (isDir) return 'var(--vsc-dir-color, #dcb67a)'
  const ext = name.split('.').pop()?.toLowerCase() ?? ''
  const map: Record<string, string> = {
    php: '#b5cea8', ts: '#4ec9b0', tsx: '#4ec9b0',
    js: '#dcdcaa', jsx: '#dcdcaa',
    scss: '#ce9178', css: '#ce9178',
    json: '#9cdcfe', md: '#cccccc', sh: '#c586c0',
  }
  return map[ext] ?? '#cccccc'
}

const FileTree: React.FC<FileTreeProps> = ({ rootPath }) => {
  const [nodeStates, setNodeStates] = useState<Map<string, NodeState>>(new Map())

  const loadChildren = useCallback(async (path: string) => {
    const entries = await window.auxilius.fs.listDir(path)
    setNodeStates(prev => {
      const next = new Map(prev)
      next.set(path, { expanded: true, children: entries })
      return next
    })
  }, [])

  // Auto-expand root on mount
  useEffect(() => {
    loadChildren(rootPath)
  }, [rootPath, loadChildren])

  const toggle = useCallback(async (path: string, isDir: boolean) => {
    if (!isDir) return
    const state = nodeStates.get(path)
    if (!state || state.children === null) {
      await loadChildren(path)
    } else {
      setNodeStates(prev => {
        const next = new Map(prev)
        next.set(path, { ...state, expanded: !state.expanded })
        return next
      })
    }
  }, [nodeStates, loadChildren])

  const renderNodes = (nodes: TreeNode[], depth: number): React.ReactNode =>
    nodes.map(node => {
      const state = nodeStates.get(node.path)
      const expanded = state?.expanded ?? false
      return (
        <React.Fragment key={node.path}>
          <div
            className={`tree-row${node.isDir ? ' tree-dir' : ''}`}
            style={{ paddingLeft: 12 + depth * 16 }}
            onClick={() => toggle(node.path, node.isDir)}
            title={node.path}
          >
            {node.isDir && (
              <span className="tree-chevron">{expanded ? '▾' : '▸'}</span>
            )}
            <span className="tree-icon">{fileIcon(node.name, node.isDir, expanded)}</span>
            <span className="tree-name" style={{ color: fileColor(node.name, node.isDir) }}>
              {node.name}
            </span>
          </div>
          {node.isDir && expanded && state?.children && (
            renderNodes(state.children, depth + 1)
          )}
        </React.Fragment>
      )
    })

  const rootState = nodeStates.get(rootPath)
  const rootName = rootPath.split('/').filter(Boolean).pop() ?? rootPath

  return (
    <div className="file-tree">
      <div
        className="tree-row tree-dir tree-root"
        onClick={() => toggle(rootPath, true)}
        title={rootPath}
      >
        <span className="tree-chevron">{rootState?.expanded ? '▾' : '▸'}</span>
        <span className="tree-icon">{rootState?.expanded ? '📂' : '📁'}</span>
        <span className="tree-name tree-root-name">{rootName}</span>
      </div>
      {rootState?.expanded && rootState.children && renderNodes(rootState.children, 1)}
    </div>
  )
}

export default FileTree
