import React, { useEffect, useState } from 'react'
import { FolderIcon } from '../../components/common/FolderIcon'
import logo from '../../assets/logo.png'

type View = 'question' | 'existing' | 'onboarding'
type StepStatus = 'pending' | 'running' | 'done' | 'error' | 'skip'

const DEFAULT_PROJECT_PATH = '/root/projects/zeus-retail-evolution'

interface StepDef {
  id: string
  label: string
  desc: string
  windowsOnly?: boolean
  run: () => Promise<{ success?: boolean; skipped?: boolean; message?: string; error?: string; stdout?: string; stderr?: string }>
}

interface Props {
  onProjectSelected: (path: string) => void
}

const STEPS: StepDef[] = [
  {
    id: 'wsl',
    label: 'WSL + Ubuntu 22.04',
    desc: 'Ambiente Linux no Windows',
    windowsOnly: true,
    run: async () => {
      const r = await window.auxilius.environment.wslCheck()
      if (r.ubuntuAvailable) return { success: true, message: 'Ubuntu já disponível no WSL' }
      return {
        success: false,
        error:
          'Ubuntu não encontrado.\n' +
          'Execute no PowerShell (como Administrador):\n' +
          '  wsl --install -d Ubuntu-22.04\n' +
          'Depois reinicie e abra o Auxilius novamente.',
      }
    },
  },
  {
    id: 'memory',
    label: 'Memória WSL — 6 GB',
    desc: '%USERPROFILE%\\.wslconfig → memory=6GB',
    windowsOnly: true,
    run: () => window.auxilius.environment.wslMemory() as Promise<any>,
  },
  {
    id: 'root',
    label: 'Usuário padrão root',
    desc: '/etc/wsl.conf → default=root',
    run: () => window.auxilius.environment.setupWslRoot() as Promise<any>,
  },
  {
    id: 'nvm',
    label: 'NVM + Node.js 20 LTS',
    desc: 'Gerenciador de versões do Node',
    run: () => window.auxilius.environment.setupNvm() as Promise<any>,
  },
  {
    id: 'mkdir',
    label: 'Criar diretório ~/projects',
    desc: '/root/projects',
    run: () => window.auxilius.environment.mkdirProjects() as Promise<any>,
  },
  {
    id: 'clone',
    label: 'Clonar projeto Zeus',
    desc: DEFAULT_PROJECT_PATH,
    run: () => window.auxilius.environment.cloneProject() as Promise<any>,
  },
]

export const WelcomePage: React.FC<Props> = ({ onProjectSelected }) => {
  const [view, setView]                 = useState<View>('question')
  const [existingPath, setExistingPath] = useState('')
  const [existingError, setExistingError] = useState('')
  const [isLinux, setIsLinux]           = useState<boolean | null>(null)
  const [statuses, setStatuses]         = useState<Record<string, StepStatus>>({})
  const [outputs, setOutputs]           = useState<Record<string, string>>({})
  const [running, setRunning]           = useState(false)
  const [activeId, setActiveId]         = useState<string | null>(null)

  // Detect platform and auto-skip Windows-only steps when on Linux
  useEffect(() => {
    if (view !== 'onboarding' || isLinux !== null) return
    window.auxilius.environment.wslCheck()
      .then(r => {
        setIsLinux(!!r.isLinux)
        if (r.isLinux) {
          setStatuses(prev => ({ ...prev, wsl: 'skip', memory: 'skip' }))
        }
      })
      .catch(() => setIsLinux(false))
  }, [view, isLinux])

  const visibleSteps = isLinux === true
    ? STEPS.filter(s => !s.windowsOnly)
    : STEPS

  const isAllDone = visibleSteps.length > 0 &&
    visibleSteps.every(s => statuses[s.id] === 'done' || statuses[s.id] === 'skip')

  const runAll = async () => {
    setRunning(true)
    for (const step of STEPS) {
      const st = statuses[step.id]
      if (st === 'done' || st === 'skip') continue

      setActiveId(step.id)
      setStatuses(prev => ({ ...prev, [step.id]: 'running' }))

      let res: { success?: boolean; skipped?: boolean; message?: string; error?: string; stdout?: string; stderr?: string }
      try { res = await step.run() }
      catch (e: any) { res = { success: false, error: e.message } }

      const out = res.message ?? res.stdout ??
        (res.success || res.skipped ? '✓ Concluído' : (res.error ?? res.stderr ?? 'Erro desconhecido'))
      setOutputs(prev => ({ ...prev, [step.id]: out }))

      if (res.success || res.skipped) {
        setStatuses(prev => ({ ...prev, [step.id]: 'done' }))
      } else {
        setStatuses(prev => ({ ...prev, [step.id]: 'error' }))
        break
      }
    }
    setActiveId(null)
    setRunning(false)
  }

  const handleExisting = () => {
    const p = existingPath.trim()
    if (!p) { setExistingError('Informe o caminho do projeto'); return }
    if (!p.startsWith('/') && !/^\\\\wsl/i.test(p)) {
      setExistingError('Use /root/projetos/zeus ou \\\\wsl.localhost\\Ubuntu\\...'); return
    }
    onProjectSelected(p)
  }

  const browsePath = async () => {
    const dir = await window.auxilius.dialog.openDirectory()
    if (dir) { setExistingPath(dir); setExistingError('') }
  }

  const stepIcon = (id: string) => {
    switch (statuses[id]) {
      case 'done':    return <span style={{ color: '#15803d' }}>✓</span>
      case 'skip':    return <span style={{ color: '#9aa4ba' }}>—</span>
      case 'error':   return <span style={{ color: '#dc2626' }}>✗</span>
      case 'running': return <span style={{ color: '#1d4ed8' }}>⟳</span>
      default:        return <span style={{ color: '#c3cbdd' }}>○</span>
    }
  }

  const stepBadge = (id: string) => {
    switch (statuses[id]) {
      case 'done':    return <span className="step-badge s-done">OK</span>
      case 'skip':    return <span className="step-badge s-skip">pulado</span>
      case 'error':   return <span className="step-badge s-error">erro</span>
      case 'running': return <span className="step-badge s-running">em andamento…</span>
      default:        return null
    }
  }

  // ── Hero (always shown) ──────────────────────────────────────────────────────
  const hero = (
    <div className="welcome-hero">
      <div className="welcome-logo">
        <img src={logo} width="64" height="64" alt="Auxilius" style={{ borderRadius: 14 }} />
      </div>
      <h1 className="welcome-title">Auxilius</h1>
      <p className="welcome-subtitle">Zeus Retail Evolution — Developer Toolbox</p>
    </div>
  )

  // ── View: initial question ───────────────────────────────────────────────────
  if (view === 'question') {
    return (
      <div className="welcome-page">
        {hero}
        <p style={{ color: 'rgba(51,65,92,0.6)', fontSize: '1rem', marginBottom: 32, textAlign: 'center' }}>
          Você já tem o projeto Zeus Retail Evolution configurado no WSL?
        </p>
        <div className="welcome-cards" style={{ maxWidth: 560 }}>
          <div className="welcome-card" style={{ cursor: 'pointer' }} onClick={() => setView('existing')}>
            <div className="card-icon">⬡</div>
            <h2 className="card-title">Sim, já tenho</h2>
            <p className="card-desc">Informe o caminho do projeto existente para abrir o Auxilius.</p>
            <button className="welcome-btn welcome-btn--secondary">Informar caminho →</button>
          </div>
          <div className="welcome-card" style={{ cursor: 'pointer' }} onClick={() => setView('onboarding')}>
            <div className="card-icon">⚙</div>
            <h2 className="card-title">Não, instalar agora</h2>
            <p className="card-desc">Configure WSL, Ubuntu, NVM, Node 20 e clone o projeto do zero.</p>
            <button className="welcome-btn welcome-btn--primary">Configurar ambiente →</button>
          </div>
        </div>
      </div>
    )
  }

  // ── View: enter existing path ────────────────────────────────────────────────
  if (view === 'existing') {
    return (
      <div className="welcome-page">
        {hero}
        <div className="wizard-card">
          <button className="wizard-back" onClick={() => setView('question')}>← Voltar</button>
          <h3>Caminho do projeto</h3>

          <div className="card-field-group">
            <label className="field-label">Caminho WSL do zeus-retail-evolution</label>
            <div className="field-input-row">
              <input
                className={`field-input${existingError ? ' field-input--error' : ''}`}
                type="text"
                placeholder="/root/projects/zeus-retail-evolution"
                value={existingPath}
                onChange={e => { setExistingPath(e.target.value); setExistingError('') }}
                onKeyDown={e => e.key === 'Enter' && handleExisting()}
              />
              <button className="browse-btn" onClick={browsePath} title="Navegar">
                <FolderIcon size={18} />
              </button>
            </div>
            {existingError && <span className="field-error">{existingError}</span>}
          </div>

          <div className="wizard-actions">
            <button className="welcome-btn welcome-btn--primary" onClick={handleExisting}>
              Abrir Projeto
            </button>
          </div>
        </div>
      </div>
    )
  }

  // ── View: onboarding wizard ──────────────────────────────────────────────────
  return (
    <div className="welcome-page" style={{ justifyContent: 'flex-start', paddingTop: 40 }}>
      {hero}
      <div className="wizard-card">
        <button className="wizard-back" onClick={() => setView('question')}>← Voltar</button>
        <h3>Configuração do Ambiente</h3>

        {STEPS.map(step => {
          // hide Windows-only steps on Linux (once detected)
          if (step.windowsOnly && isLinux === true) return null
          const st = statuses[step.id] ?? 'pending'
          const out = outputs[step.id]
          return (
            <div key={step.id} className="wizard-step">
              <div className="step-row">
                <span className="step-icon">{stepIcon(step.id)}</span>
                <div className="step-body">
                  <div className="step-label">{step.label}</div>
                  <div className="step-desc">{step.desc}</div>
                </div>
                {stepBadge(step.id)}
              </div>

              {out && (st === 'running' || st === 'done' || st === 'error') && (
                <div className={`step-output ${st === 'error' ? 'out-error' : st === 'done' ? 'out-ok' : 'out-info'}`}>
                  {out}
                </div>
              )}

              {st === 'error' && !running && (
                <button className="step-skip-btn" onClick={() => {
                  setStatuses(prev => ({ ...prev, [step.id]: 'skip' }))
                }}>
                  Pular esta etapa
                </button>
              )}
            </div>
          )
        })}

        <div className="wizard-actions">
          {!isAllDone && (
            <button
              className="welcome-btn welcome-btn--primary"
              disabled={running || isLinux === null}
              onClick={runAll}
            >
              {running ? `Configurando… (${STEPS.find(s => s.id === activeId)?.label ?? ''})` : 'Configurar Tudo'}
            </button>
          )}

          {isAllDone && (
            <button
              className="welcome-btn welcome-btn--primary"
              onClick={() => onProjectSelected(DEFAULT_PROJECT_PATH)}
            >
              Entrar no Auxilius →
            </button>
          )}
        </div>
      </div>
    </div>
  )
}
