import React, { useEffect, useState } from 'react'
import { Button, TextField, Select } from '@giro-ds/react'
import type { FieldDef, EndpointConfig, GeneratedFile } from '../../types/electron'
import { useProject } from '../../contexts/ProjectContext'
import { FilePreview } from './components/FilePreview'

const DEFAULT_CONTEXTS = [
  'Autenticacao', 'Clientes', 'Compartilhado', 'Devolucoes', 'Lojas',
  'Mercadorias', 'Promocoes', 'Vendas', 'Tesouraria', 'Finalizadoras',
  'Funcionarios', 'Pdvs', 'Combos', 'Documentacao'
]

const OPERATIONS = [
  { value: 'criar',     label: 'criar — POST',       method: 'POST'   },
  { value: 'alterar',   label: 'alterar — PUT',       method: 'PUT'    },
  { value: 'deletar',   label: 'deletar — DELETE',    method: 'DELETE' },
  { value: 'consultar', label: 'consultar — GET list', method: 'GET'   },
  { value: 'detalhar',  label: 'detalhar — GET by ID', method: 'GET'   },
]

const PHP_TYPES = ['string', 'int', 'float', 'bool']

const VO_SUGGESTIONS: Record<string, string> = {
  id:      'IdVO',
  email:   'EmailVO',
  cpf:     'CPFVO',
  cnpj:    'CNPJVO',
  phone:   'PhoneVO',
  cep:     'CepVO',
  preco:   'MoneyVO',
  valor:   'MoneyVO',
  total:   'MoneyVO',
  decimal: 'DecimalVO',
}

function suggestVO(name: string, phpType: string): string {
  const lower = name.toLowerCase()
  for (const [key, vo] of Object.entries(VO_SUGGESTIONS)) {
    if (lower.includes(key)) return vo
  }
  return phpType === 'int' ? 'int' : phpType === 'float' ? 'float' : phpType === 'bool' ? 'bool' : 'string'
}

function emptyField(): FieldDef {
  return { name: '', phpType: 'string', voType: 'string', nullable: false }
}

export const EndpointsPage: React.FC = () => {
  const { projectPath } = useProject()
  const [contexts, setContexts]       = useState<string[]>(DEFAULT_CONTEXTS)
  const [context, setContext]         = useState('')
  const [resource, setResource]       = useState('')
  const [operation, setOperation]     = useState<EndpointConfig['operation']>('criar')
  const [fields, setFields]           = useState<FieldDef[]>([emptyField()])
  const [previewing, setPreviewing]   = useState(false)
  const [generating, setGenerating]   = useState(false)
  const [previewFiles, setPreviewFiles] = useState<GeneratedFile[]>([])
  const [output, setOutput]           = useState('')
  const [isError, setIsError]         = useState(false)

  useEffect(() => {
    if (!projectPath) return
    window.auxilius.endpoints.listContexts(projectPath).then(list => {
      if (list.length > 0) setContexts(list)
    })
  }, [projectPath])

  const buildConfig = (): EndpointConfig => ({
    projectPath,
    context,
    resource,
    operation,
    fields: fields.filter(f => f.name.trim() !== '')
  })

  const handlePreview = async () => {
    setPreviewing(true)
    setOutput('')
    const r = await window.auxilius.endpoints.preview(buildConfig())
    if (r.success && r.files) {
      setPreviewFiles(r.files)
    } else {
      setIsError(true)
      setOutput(`✗ Erro ao gerar preview: ${r.error}`)
    }
    setPreviewing(false)
  }

  const handleGenerate = async () => {
    setGenerating(true)
    setPreviewFiles([])
    setIsError(false)
    const r = await window.auxilius.endpoints.generate(buildConfig())
    if (r.success) {
      setOutput(
        [
          '✓ Arquivos gerados:',
          ...(r.written ?? []).map(f => `  + ${f}`),
          ...(r.skipped ?? []).length > 0 ? ['\n⚠ Arquivos já existentes (não sobrescritos):'] : [],
          ...(r.skipped ?? []).map(f => `  ~ ${f}`),
        ].join('\n')
      )
    } else {
      setIsError(true)
      setOutput(`✗ Erro: ${r.error}`)
    }
    setGenerating(false)
  }

  const updateField = (idx: number, patch: Partial<FieldDef>) => {
    setFields(prev => prev.map((f, i) => {
      if (i !== idx) return f
      const updated = { ...f, ...patch }
      if (patch.name !== undefined || patch.phpType !== undefined) {
        updated.voType = suggestVO(updated.name, updated.phpType)
      }
      return updated
    }))
  }

  const contextItems   = contexts.map(c => ({ value: c, text: c }))
  const operationItems = OPERATIONS.map(o => ({ value: o.value, text: o.label }))
  const phpTypeItems   = PHP_TYPES.map(t => ({ value: t, text: t }))

  const isValid = context && resource && fields.some(f => f.name.trim() !== '')

  return (
    <>
      <div className="page-header">
        <h2>Criar Endpoint DDD</h2>
        <p>Gere UseCase/Query, DTOs, Entity, Repository e Route para um novo endpoint</p>
      </div>

      {/* Project path */}
      <div className="form-section">
        <h3>Projeto</h3>
        <p style={{ fontSize: '0.8rem', color: '#6b7280', fontFamily: 'monospace' }}>{projectPath}</p>
      </div>

      {/* Endpoint config */}
      <div className="form-section">
        <h3>Configuração do Endpoint</h3>
        <div className="form-row">
          <Select
            variant="text"
            label="Contexto DDD"
            placeholder="Selecione o contexto"
            items={contextItems}
            value={context}
            onValueChange={(v: string | string[]) => setContext(v as string)}
            search
          />
          <TextField
            label="Nome do recurso"
            placeholder="Ex: Cliente, Venda, Produto"
            value={resource}
            onChange={(value: string) => setResource(value)}
          />
          <Select
            variant="text"
            label="Operação"
            placeholder="Selecione a operação"
            items={operationItems}
            value={operation}
            onValueChange={(v: string | string[]) => setOperation(v as EndpointConfig['operation'])}
          />
        </div>
      </div>

      {/* Fields */}
      <div className="form-section">
        <h3>Campos do recurso</h3>
        <div className="fields-builder">
          {fields.map((field, idx) => (
            <div key={idx} className="field-row">
              <TextField
                label={idx === 0 ? 'Nome do campo' : undefined}
                placeholder="nome_campo"
                value={field.name}
                onChange={(value: string) => updateField(idx, { name: value })}
              />
              <Select
                variant="text"
                label={idx === 0 ? 'Tipo PHP' : undefined}
                items={phpTypeItems}
                value={field.phpType}
                onValueChange={(v: string | string[]) => updateField(idx, { phpType: v as FieldDef['phpType'] })}
              />
              <TextField
                label={idx === 0 ? 'Value Object' : undefined}
                placeholder="string"
                value={field.voType}
                onChange={(value: string) => updateField(idx, { voType: value })}
              />
              <div style={{ display: 'flex', gap: 6, alignItems: 'flex-end' }}>
                <label style={{ display: 'flex', alignItems: 'center', gap: 4, fontSize: '0.8rem', marginBottom: 4 }}>
                  <input
                    type="checkbox"
                    checked={field.nullable}
                    onChange={e => updateField(idx, { nullable: e.target.checked })}
                  />
                  ?
                </label>
                {fields.length > 1 && (
                  <Button
                    variant="text"
                    onClick={() => setFields(prev => prev.filter((_, i) => i !== idx))}
                  >
                    ✕
                  </Button>
                )}
              </div>
            </div>
          ))}

          <Button
            variant="outlined"
            className="add-field-btn"
            onClick={() => setFields(prev => [...prev, emptyField()])}
          >
            + Adicionar campo
          </Button>
        </div>
      </div>

      {/* Actions */}
      <div className="action-bar">
        <Button
          variant="outlined"
          disabled={!isValid || previewing || generating}
          onClick={handlePreview}
        >
          {previewing ? 'Gerando preview…' : '👁 Visualizar arquivos'}
        </Button>
        <Button
          variant="filled"
          disabled={!isValid || previewing || generating}
          onClick={handleGenerate}
        >
          {generating ? 'Gerando…' : '⊕ Gerar endpoint'}
        </Button>
      </div>

      {previewFiles.length > 0 && (
        <FilePreview files={previewFiles} />
      )}

      {output && (
        <div className="terminal" style={{ marginTop: 16 }}>
          <span className={isError ? 't-error' : 't-success'}>{output}</span>
        </div>
      )}
    </>
  )
}
