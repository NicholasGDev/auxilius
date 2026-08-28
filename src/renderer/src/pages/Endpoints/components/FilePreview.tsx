import React, { useState } from 'react'
import type { GeneratedFile } from '../../../types/electron'

interface Props {
  files: GeneratedFile[]
}

// ─── Simple PHP syntax highlighter ───────────────────────────────────────────

const KEYWORDS = new Set([
  'class','interface','abstract','readonly','extends','implements',
  'namespace','use','function','return','new','if','else','elseif',
  'foreach','for','while','throw','catch','try','finally',
  'null','true','false','static','public','private','protected',
  'void','string','int','float','bool','array','object','self','parent',
  'declare','echo','print','match','fn',
])

function esc(s: string): string {
  return s.replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;')
}

function highlightPhp(code: string): string {
  let result = ''
  let i = 0
  const len = code.length

  while (i < len) {
    // PHP open tag
    if (code.startsWith('<?php', i)) {
      result += `<span style="color:#569cd6">&lt;?php</span>`
      i += 5
      continue
    }
    // Line comment //
    if (code[i] === '/' && i + 1 < len && code[i + 1] === '/') {
      const end = code.indexOf('\n', i)
      const slice = end === -1 ? code.slice(i) : code.slice(i, end)
      result += `<span style="color:#6a9955">${esc(slice)}</span>`
      i = end === -1 ? len : end
      continue
    }
    // Block comment /* */
    if (code[i] === '/' && i + 1 < len && code[i + 1] === '*') {
      const end = code.indexOf('*/', i + 2)
      const slice = end === -1 ? code.slice(i) : code.slice(i, end + 2)
      result += `<span style="color:#6a9955">${esc(slice)}</span>`
      i = end === -1 ? len : end + 2
      continue
    }
    // Single-quoted string
    if (code[i] === "'") {
      let j = i + 1
      while (j < len && code[j] !== "'") { if (code[j] === '\\') j++; j++ }
      j++
      result += `<span style="color:#ce9178">${esc(code.slice(i, j))}</span>`
      i = j
      continue
    }
    // Double-quoted string
    if (code[i] === '"') {
      let j = i + 1
      while (j < len && code[j] !== '"') { if (code[j] === '\\') j++; j++ }
      j++
      result += `<span style="color:#ce9178">${esc(code.slice(i, j))}</span>`
      i = j
      continue
    }
    // Variable $name
    if (code[i] === '$') {
      let j = i + 1
      while (j < len && /[a-zA-Z0-9_]/.test(code[j])) j++
      result += `<span style="color:#9cdcfe">${esc(code.slice(i, j))}</span>`
      i = j
      continue
    }
    // Word (keyword / class name / identifier)
    if (/[a-zA-Z_]/.test(code[i])) {
      let j = i + 1
      while (j < len && /[a-zA-Z0-9_\\]/.test(code[j])) j++
      const word = code.slice(i, j)
      if (KEYWORDS.has(word)) {
        result += `<span style="color:#569cd6">${esc(word)}</span>`
      } else if (/^[A-Z]/.test(word)) {
        result += `<span style="color:#4ec9b0">${esc(word)}</span>`
      } else {
        result += esc(word)
      }
      i = j
      continue
    }
    result += esc(code[i])
    i++
  }
  return result
}

// ─── Component ───────────────────────────────────────────────────────────────

const STYLES = {
  container: {
    display: 'flex' as const,
    height: 480,
    border: '1px solid #2d2d2d',
    borderRadius: 4,
    overflow: 'hidden',
    fontFamily: "'Consolas', 'Monaco', 'Courier New', monospace",
    fontSize: 12,
    marginTop: 16,
  },
  sidebar: {
    width: 220,
    background: '#252526',
    borderRight: '1px solid #2d2d2d',
    overflowY: 'auto' as const,
    flexShrink: 0,
  },
  sidebarHeader: {
    padding: '8px 12px',
    fontSize: 11,
    fontWeight: 600,
    color: '#bbb',
    textTransform: 'uppercase' as const,
    letterSpacing: 1,
    borderBottom: '1px solid #2d2d2d',
  },
  treeItem: (active: boolean, isNew: boolean) => ({
    padding: '4px 8px 4px 16px',
    cursor: 'pointer' as const,
    color: active ? '#ffffff' : isNew ? '#73c991' : '#e0c88e',
    background: active ? '#094771' : 'transparent',
    whiteSpace: 'nowrap' as const,
    overflow: 'hidden',
    textOverflow: 'ellipsis',
    display: 'flex',
    alignItems: 'center',
    gap: 6,
    fontSize: 12,
  }),
  editor: {
    flex: 1,
    background: '#1e1e1e',
    overflowY: 'auto' as const,
    display: 'flex' as const,
    flexDirection: 'column' as const,
  },
  editorHeader: {
    background: '#2d2d2d',
    padding: '4px 12px',
    fontSize: 11,
    color: '#9d9d9d',
    borderBottom: '1px solid #3d3d3d',
    display: 'flex',
    alignItems: 'center',
    justifyContent: 'space-between',
  },
  codeWrapper: {
    display: 'flex' as const,
    flex: 1,
    overflowY: 'auto' as const,
  },
  lineNumbers: {
    padding: '12px 8px',
    textAlign: 'right' as const,
    color: '#3d3d3d',
    userSelect: 'none' as const,
    background: '#1e1e1e',
    minWidth: 36,
    flexShrink: 0,
    lineHeight: '18px',
    fontSize: 12,
  },
  code: {
    padding: '12px 16px',
    color: '#d4d4d4',
    whiteSpace: 'pre' as const,
    overflowX: 'auto' as const,
    flex: 1,
    lineHeight: '18px',
    fontSize: 12,
  },
}

function fileIcon(path: string): string {
  if (path.endsWith('.php')) return '📄'
  if (path.includes('Entity')) return '🔷'
  if (path.includes('Repository')) return '🗄️'
  if (path.includes('UseCase')) return '⚡'
  if (path.includes('Query')) return '🔍'
  if (path.includes('Exception')) return '⚠️'
  if (path.includes('Error')) return '🔴'
  if (path.includes('DTO') || path.includes('Input') || path.includes('Output')) return '📦'
  return '📄'
}

export const FilePreview: React.FC<Props> = ({ files }) => {
  const [active, setActive] = useState(0)

  if (files.length === 0) return null

  const current        = files[active]
  const lines          = current.content.split('\n')
  const highlighted    = highlightPhp(current.content)
  const highlightLines = highlighted.split('\n')

  return (
    <div>
      <div style={{ display: 'flex', alignItems: 'center', justifyContent: 'space-between', marginBottom: 8 }}>
        <span style={{ fontSize: 13, fontWeight: 600 }}>
          Preview — {files.length} arquivo{files.length !== 1 ? 's' : ''}
        </span>
        <span style={{ fontSize: 11, color: '#6b7280' }}>
          {files.filter(f => f.isNew).length} novo{files.filter(f => f.isNew).length !== 1 ? 's' : ''} &nbsp;·&nbsp;
          {files.filter(f => !f.isNew).length} já existe{files.filter(f => !f.isNew).length !== 1 ? 'm' : ''}
        </span>
      </div>

      <div style={STYLES.container}>
        {/* File tree */}
        <div style={STYLES.sidebar}>
          <div style={STYLES.sidebarHeader}>EXPLORER</div>
          {files.map((f, idx) => (
            <div
              key={idx}
              style={STYLES.treeItem(idx === active, f.isNew)}
              onClick={() => setActive(idx)}
              title={f.relativePath}
            >
              <span style={{ fontSize: 10 }}>{fileIcon(f.relativePath)}</span>
              <span style={{ overflow: 'hidden', textOverflow: 'ellipsis' }}>
                {f.relativePath.split('/').pop()}
              </span>
            </div>
          ))}
        </div>

        {/* Editor */}
        <div style={STYLES.editor}>
          <div style={STYLES.editorHeader}>
            <span title={current.relativePath} style={{ overflow: 'hidden', textOverflow: 'ellipsis', maxWidth: '60%' }}>
              {current.relativePath}
            </span>
            <span style={{
              padding: '1px 6px',
              borderRadius: 3,
              fontSize: 10,
              fontWeight: 600,
              background: current.isNew ? '#1a3a1a' : '#3a2e00',
              color: current.isNew ? '#73c991' : '#e0c88e',
            }}>
              {current.isNew ? '+ NOVO' : '~ EXISTENTE'}
            </span>
          </div>

          <div style={STYLES.codeWrapper}>
            <div style={STYLES.lineNumbers}>
              {lines.map((_: string, i: number) => (
                <div key={i}>{i + 1}</div>
              ))}
            </div>
            <pre
              style={STYLES.code}
              dangerouslySetInnerHTML={{ __html: highlightLines.join('\n') }}
            />
          </div>
        </div>
      </div>
    </div>
  )
}

