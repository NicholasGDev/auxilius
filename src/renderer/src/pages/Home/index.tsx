import React, { useEffect, useState } from 'react'
import { Button } from '@giro-ds/react'
import type { EnvironmentStatus } from '../../types/electron'

export const HomePage: React.FC = () => {
  const [status, setStatus] = useState<EnvironmentStatus | null>(null)
  const [loading, setLoading] = useState(true)

  const refresh = async () => {
    setLoading(true)
    const result = await window.auxilius.environment.check()
    setStatus(result)
    setLoading(false)
  }

  useEffect(() => { refresh() }, [])

  // The main process may fall back to an error shape (e.g. binary/WSL failure);
  // guard against missing fields instead of crashing the whole page.
  const items = status?.git
    ? [
        { label: 'Git',          ok: status.git.ok,          detail: status.git.version ?? status.git.message },
        { label: 'NVM',          ok: status.nvm.ok,          detail: status.nvm.version ?? status.nvm.message },
        { label: 'Node 20',      ok: status.node20.ok,       detail: status.node20.version ?? status.node20.message },
        { label: 'Node 19',      ok: status.node19.ok,       detail: status.node19.version ?? status.node19.message },
        { label: 'WSL root',     ok: status.rootDefault,     detail: status.rootDefault ? 'default=root' : 'Não configurado' },
        { label: 'Projeto Zeus', ok: status.projectExists,   detail: status.projectPath },
      ]
    : []

  return (
    <>
      <div className="page-header">
        <h2>Dashboard</h2>
        <p>Visão geral do ambiente de desenvolvimento</p>
      </div>

      <div className="form-section" style={{ marginBottom: 24 }}>
        <div style={{ display: 'flex', justifyContent: 'space-between', alignItems: 'center', marginBottom: 16 }}>
          <h3>Status do Ambiente</h3>
          <Button variant="outlined" onClick={refresh} disabled={loading}>
            {loading ? 'Verificando…' : 'Atualizar'}
          </Button>
        </div>

        {loading && <p style={{ color: '#6b7280', fontSize: '0.875rem' }}>Verificando ambiente…</p>}

        {!loading && items.map(item => (
          <div key={item.label} className="check-item">
            <div className="check-label">
              {item.label}
              <span>{item.detail}</span>
            </div>
            <span className={`badge ${item.ok ? 'badge-ok' : 'badge-error'}`}>
              {item.ok ? '✓ OK' : '✗ Ausente'}
            </span>
          </div>
        ))}
      </div>

      <div className="card-grid">
        {[
          { title: 'Configurar Ambiente', desc: 'WSL, Git, NVM e Node', path: '/setup', color: '#3b82f6' },
          { title: 'Compilar Scaffold',   desc: 'Gerar estrutura do projeto', path: '/scaffold', color: '#8b5cf6' },
          { title: 'Criar Endpoint',      desc: 'Novo endpoint DDD via GUI', path: '/endpoints', color: '#10b981' },
        ].map(card => (
          <div key={card.title} className="status-card" style={{ borderTop: `3px solid ${card.color}` }}>
            <div className="status-card-title">{card.title}</div>
            <div className="status-card-value" style={{ marginBottom: 12 }}>{card.desc}</div>
            <Button variant="outlined" onClick={() => window.location.hash = card.path}>
              Acessar →
            </Button>
          </div>
        ))}
      </div>
    </>
  )
}
