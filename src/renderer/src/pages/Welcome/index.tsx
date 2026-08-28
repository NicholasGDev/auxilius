import React, { useState } from 'react'
import { FolderIcon } from '../../components/common/FolderIcon'

interface Props {
  onProjectSelected: (path: string) => void
}

export const WelcomePage: React.FC<Props> = ({ onProjectSelected }) => {
  const [newPath, setNewPath]         = useState('')
  const [existingPath, setExistingPath] = useState('')
  const [newError, setNewError]       = useState('')
  const [existingError, setExistingError] = useState('')

  const isValidPath = (p: string) => p.startsWith('/') || /^\\\\wsl/i.test(p)

  const handleNew = () => {
    const p = newPath.trim()
    if (!p) { setNewError('Informe o caminho do diretório'); return }
    if (!isValidPath(p)) { setNewError('Use /root/projetos/zeus ou \\\\wsl.localhost\\Ubuntu\\root\\projetos\\zeus'); return }
    setNewError('')
    onProjectSelected(p)
  }

  const handleExisting = () => {
    const p = existingPath.trim()
    if (!p) { setExistingError('Informe o caminho do diretório'); return }
    if (!isValidPath(p)) { setExistingError('Use /root/projetos/zeus ou \\\\wsl.localhost\\Ubuntu\\root\\projetos\\zeus'); return }
    setExistingError('')
    onProjectSelected(p)
  }

  const browsePath = async (setter: (v: string) => void, clearError: () => void) => {
    const dir = await window.auxilius.dialog.openDirectory()
    if (dir) { setter(dir); clearError() }
  }

  return (
    <div className="welcome-page">
      <div className="welcome-hero">
        <div className="welcome-logo">
          <svg width="64" height="64" viewBox="0 0 64 64" fill="none" xmlns="http://www.w3.org/2000/svg">
            <circle cx="32" cy="32" r="32" fill="#0f172a" />
            {/* Body — curly braces */}
            <text x="50%" y="56%" dominantBaseline="middle" textAnchor="middle"
              fontFamily="monospace" fontSize="30" fontWeight="bold" fill="#38bdf8">
              {'{}'}
            </text>
            {/* Halo */}
            <ellipse cx="32" cy="10" rx="14" ry="4" stroke="#38bdf8" strokeWidth="2" fill="none" />
            {/* Wings */}
            <path d="M10 32 Q4 24 10 20 Q16 28 18 32 Z" fill="#1e40af" opacity="0.8" />
            <path d="M54 32 Q60 24 54 20 Q48 28 46 32 Z" fill="#1e40af" opacity="0.8" />
            {/* Circuit lines */}
            <line x1="22" y1="38" x2="18" y2="44" stroke="#38bdf8" strokeWidth="1" opacity="0.5" />
            <line x1="42" y1="38" x2="46" y2="44" stroke="#38bdf8" strokeWidth="1" opacity="0.5" />
          </svg>
        </div>
        <h1 className="welcome-title">Auxilius</h1>
        <p className="welcome-subtitle">Zeus Retail Evolution — Developer Toolbox</p>
      </div>

      <div className="welcome-cards">
        {/* ── Card: Novo Projeto ──────────────────────────── */}
        <div className="welcome-card">
          <div className="card-icon">◈</div>
          <h2 className="card-title">Novo Projeto</h2>
          <p className="card-desc">
            Gere a estrutura completa de um novo projeto Zeus Retail Evolution do zero.
          </p>
          <div className="card-field-group">
            <label className="field-label">Diretório destino (WSL)</label>
            <div className="field-input-row">
              <input
                className={`field-input${newError ? ' field-input--error' : ''}`}
                type="text"
                placeholder="/root/projetos/zeus-retail ou \\wsl.localhost\Ubuntu\root\projetos"
                value={newPath}
                onChange={e => { setNewPath(e.target.value); setNewError('') }}
                onKeyDown={e => e.key === 'Enter' && handleNew()}
              />
              <button className="browse-btn" onClick={() => browsePath(setNewPath, () => setNewError(''))} title="Navegar">
                <FolderIcon size={18} />
              </button>
            </div>
            {newError && <span className="field-error">{newError}</span>}
          </div>
          <button className="welcome-btn welcome-btn--primary" onClick={handleNew}>
            Criar Projeto
          </button>
        </div>

        {/* ── Card: Projeto Existente ─────────────────────── */}
        <div className="welcome-card">
          <div className="card-icon">⬡</div>
          <h2 className="card-title">Projeto Existente</h2>
          <p className="card-desc">
            Conecte-se a um projeto Zeus já existente para gerar endpoints e gerenciar o ambiente.
          </p>
          <div className="card-field-group">
            <label className="field-label">Caminho do projeto (WSL)</label>
            <div className="field-input-row">
              <input
                className={`field-input${existingError ? ' field-input--error' : ''}`}
                type="text"
                placeholder="/root/projetos/zeus-retail-evolution ou \\wsl.localhost\Ubuntu\root\projetos"
                value={existingPath}
                onChange={e => { setExistingPath(e.target.value); setExistingError('') }}
                onKeyDown={e => e.key === 'Enter' && handleExisting()}
              />
              <button className="browse-btn" onClick={() => browsePath(setExistingPath, () => setExistingError(''))} title="Navegar">
                <FolderIcon size={18} />
              </button>
            </div>
            {existingError && <span className="field-error">{existingError}</span>}
          </div>
          <button className="welcome-btn welcome-btn--secondary" onClick={handleExisting}>
            Abrir Projeto
          </button>
        </div>
      </div>

      <p className="welcome-footer">
        Caminhos WSL suportados: <code>/root/…</code>, <code>/home/user/…</code>, <code>/mnt/c/…</code>
      </p>
    </div>
  )
}
