import React, { useEffect, useState } from 'react'
import { Button, TextField } from '@giro-ds/react'
import type { EnvironmentStatus, CommandResult } from '../../types/electron'

type Step = 'wsl' | 'git' | 'nvm' | 'project'

export const SetupPage: React.FC = () => {
  const [status, setStatus]     = useState<EnvironmentStatus | null>(null)
  const [loading, setLoading]   = useState(true)
  const [running, setRunning]   = useState<Step | null>(null)
  const [output, setOutput]     = useState('')
  const [gitName, setGitName]   = useState('')
  const [gitEmail, setGitEmail] = useState('')

  const refresh = async () => {
    setLoading(true)
    const s = await window.auxilius.environment.check()
    setStatus(s)
    setLoading(false)
  }

  useEffect(() => { refresh() }, [])

  const run = async (step: Step, action: () => Promise<CommandResult>) => {
    setRunning(step)
    setOutput(`→ Executando ${step}…\n`)
    const result = await action()
    setOutput(
      result.success
        ? `✓ Sucesso\n${result.stdout ?? ''}\n${result.message ?? ''}`
        : `✗ Erro: ${result.error}\n${result.stderr ?? ''}`
    )
    setRunning(null)
    refresh()
  }

  const okIcon = (ok: boolean) => (
    <span className={`badge ${ok ? 'badge-ok' : 'badge-error'}`}>
      {ok ? '✓ OK' : '✗ Ausente'}
    </span>
  )

  return (
    <>
      <div className="page-header">
        <h2>Configurar Ambiente</h2>
        <p>Configure WSL, Git, NVM e Node 20 para desenvolvimento</p>
      </div>

      <div className="form-section">
        <div style={{ display: 'flex', justifyContent: 'space-between', marginBottom: 16 }}>
          <h3>Checklist</h3>
          <Button variant="text" onClick={refresh} disabled={loading}>
            {loading ? 'Verificando…' : '↺ Atualizar'}
          </Button>
        </div>

        {/* WSL default root */}
        <div className="check-item">
          <div className="check-label">
            WSL — usuário padrão root
            <span>Configura /etc/wsl.conf com default=root</span>
          </div>
          <div style={{ display: 'flex', gap: 8, alignItems: 'center' }}>
            {status && okIcon(status.rootDefault)}
            <Button
              variant="outlined"
              disabled={running !== null || status?.rootDefault}
              onClick={() => run('wsl', () => window.auxilius.environment.setupWslRoot())}
            >
              {running === 'wsl' ? 'Aguarde…' : 'Configurar'}
            </Button>
          </div>
        </div>

        {/* Git */}
        <div className="check-item">
          <div className="check-label">
            Git
            <span>{status?.git.version ?? 'Verificando…'}</span>
          </div>
          {status && okIcon(status.git.ok)}
        </div>

        {/* Git user config */}
        <div className="check-item" style={{ flexDirection: 'column', alignItems: 'flex-start', gap: 12 }}>
          <div className="check-label">
            Git — Configurar usuário global
            <span>git config --global user.name / user.email</span>
          </div>
          <div style={{ display: 'grid', gridTemplateColumns: '1fr 1fr auto', gap: 12, width: '100%' }}>
            <TextField
              label="Nome"
              placeholder="Seu nome"
              value={gitName}
              onChange={(value: string) => setGitName(value)}
            />
            <TextField
              label="E-mail"
              placeholder="seu@email.com"
              value={gitEmail}
              onChange={(value: string) => setGitEmail(value)}
            />
            <div style={{ display: 'flex', alignItems: 'flex-end' }}>
              <Button
                variant="outlined"
                disabled={running !== null || !gitName || !gitEmail}
                onClick={() => run('git', () =>
                  window.auxilius.environment.configureGit(gitName, gitEmail)
                )}
              >
                {running === 'git' ? 'Aguarde…' : 'Salvar'}
              </Button>
            </div>
          </div>
        </div>

        {/* NVM + Node */}
        <div className="check-item">
          <div className="check-label">
            NVM + Node 19 e 20 (padrão: 20)
            <span>
              NVM: {status?.nvm.version ?? '…'} | Node 20: {status?.node20.version ?? '…'} | Node 19: {status?.node19.version ?? '…'}
            </span>
          </div>
          <div style={{ display: 'flex', gap: 8, alignItems: 'center' }}>
            {status && okIcon(status.nvm.ok && status.node20.ok)}
            <Button
              variant="outlined"
              disabled={running !== null || (status?.nvm.ok && status?.node20.ok && status?.node19.ok)}
              onClick={() => run('nvm', () => window.auxilius.environment.setupNvm())}
            >
              {running === 'nvm' ? 'Instalando…' : 'Instalar'}
            </Button>
          </div>
        </div>

        {/* Clone project */}
        <div className="check-item">
          <div className="check-label">
            Clonar projeto Zeus Retail Evolution
            <span>{status?.projectPath ?? '~/projects/zeus-retail-evolution'}</span>
          </div>
          <div style={{ display: 'flex', gap: 8, alignItems: 'center' }}>
            {status && okIcon(status.projectExists)}
            <Button
              variant="outlined"
              disabled={running !== null || status?.projectExists}
              onClick={() => run('project', () => window.auxilius.environment.cloneProject())}
            >
              {running === 'project' ? 'Clonando…' : 'Clonar'}
            </Button>
          </div>
        </div>
      </div>

      {output && (
        <div className="terminal">
          {output}
        </div>
      )}
    </>
  )
}
