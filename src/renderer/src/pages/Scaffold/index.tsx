import React, { useEffect, useState } from 'react'
import { Button, TextField } from '@giro-ds/react'
import { homedir } from 'os'

export const ScaffoldPage: React.FC = () => {
  const [srcPath, setSrcPath]     = useState('')
  const [srcExists, setSrcExists] = useState(false)
  const [targetDir, setTargetDir] = useState('')
  const [compiled, setCompiled]   = useState(false)
  const [loading, setLoading]     = useState<'compile' | 'run' | null>(null)
  const [output, setOutput]       = useState('')
  const [isError, setIsError]     = useState(false)

  useEffect(() => {
    window.auxilius.scaffold.getInfo().then(info => {
      setSrcPath(info.srcPath)
      setSrcExists(info.exists)
      // Default output dir one level above the app path
      setTargetDir('/root/projects')
    })
  }, [])

  const compile = async () => {
    setLoading('compile')
    setOutput('→ Compilando scaffold_zeus.cpp com g++ -std=c++20…\n')
    setIsError(false)
    const r = await window.auxilius.scaffold.compile()
    if (r.success) {
      setCompiled(true)
      setOutput(`✓ Compilado em: ${r.binaryPath}\n${r.stderr ?? ''}`)
    } else {
      setIsError(true)
      setOutput(`✗ Erro de compilação:\n${r.stderr ?? r.error}`)
    }
    setLoading(null)
  }

  const run = async () => {
    setLoading('run')
    setOutput(`→ Executando scaffold em: ${targetDir}\n`)
    setIsError(false)
    const r = await window.auxilius.scaffold.run(targetDir)
    if (r.success) {
      setOutput(`✓ Estrutura gerada com sucesso!\n${r.stdout ?? ''}`)
    } else {
      setIsError(true)
      setOutput(`✗ Falha na execução:\n${r.error}\n${r.stderr ?? ''}`)
    }
    setLoading(null)
  }

  return (
    <>
      <div className="page-header">
        <h2>Scaffold do Projeto</h2>
        <p>Compile e execute o gerador de estrutura C++ para o Zeus Retail Evolution</p>
      </div>

      <div className="form-section">
        <h3>Código fonte</h3>
        <div className="check-item">
          <div className="check-label">
            scaffold_zeus.cpp
            <span style={{ wordBreak: 'break-all' }}>{srcPath}</span>
          </div>
          <span className={`badge ${srcExists ? 'badge-ok' : 'badge-error'}`}>
            {srcExists ? '✓ Encontrado' : '✗ Ausente'}
          </span>
        </div>
      </div>

      <div className="form-section">
        <h3>Compilação</h3>
        <p style={{ fontSize: '0.875rem', color: '#6b7280', marginBottom: 16 }}>
          Requer <code>g++</code> com suporte a C++20 instalado no sistema.
        </p>
        <Button
          variant="filled"
          disabled={!srcExists || loading !== null}
          onClick={compile}
        >
          {loading === 'compile' ? 'Compilando…' : '⚙ Compilar scaffold_zeus.cpp'}
        </Button>
      </div>

      <div className="form-section">
        <h3>Executar gerador</h3>
        <p style={{ fontSize: '0.875rem', color: '#6b7280', marginBottom: 16 }}>
          O scaffold cria o diretório <code>ZeusRetailEvolution/</code> dentro do
          diretório escolhido abaixo.
        </p>
        <TextField
          label="Diretório de saída"
          placeholder="/root/projects"
          value={targetDir}
          onChange={(value: string) => setTargetDir(value)}
        />
        <div className="action-bar">
          <Button
            variant="filled"
            disabled={!compiled || !targetDir || loading !== null}
            onClick={run}
          >
            {loading === 'run' ? 'Gerando…' : '▶ Gerar estrutura'}
          </Button>
          {!compiled && (
            <span className="badge badge-warning">Compile antes de executar</span>
          )}
        </div>
      </div>

      {output && (
        <div className="terminal">
          <span className={isError ? 't-error' : 't-success'}>{output}</span>
        </div>
      )}
    </>
  )
}
