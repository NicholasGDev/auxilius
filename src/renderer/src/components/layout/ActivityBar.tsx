import React from 'react'

export type ActivitySection = 'explorer' | 'scaffold' | 'endpoints' | 'setup' | 'welcome'

interface ActivityItemProps {
  section: ActivitySection
  active:  ActivitySection
  label:   string
  icon:    React.ReactNode
  onClick: (s: ActivitySection) => void
}

const ActivityItem: React.FC<ActivityItemProps> = ({ section, active, label, icon, onClick }) => (
  <div
    className={`activity-item${active === section ? ' active' : ''}`}
    onClick={() => onClick(section)}
    role="button"
    aria-label={label}
  >
    {icon}
    <span className="activity-tooltip">{label}</span>
  </div>
)

interface ActivityBarProps {
  active:    ActivitySection
  onChange:  (s: ActivitySection) => void
  onSwitch:  () => void
}

export const ActivityBar: React.FC<ActivityBarProps> = ({ active, onChange, onSwitch }) => (
  <aside className="vsc-activity-bar">
    <ActivityItem section="explorer"  active={active} label="Explorer"  onClick={onChange}
      icon={<svg viewBox="0 0 16 16" fill="currentColor"><path d="M13.5 3h-4l-1.5-2h-4A1.5 1.5 0 0 0 2.5 2.5v11A1.5 1.5 0 0 0 4 15h9.5a1.5 1.5 0 0 0 1.5-1.5v-9A1.5 1.5 0 0 0 13.5 3z"/></svg>}
    />
    <ActivityItem section="scaffold"  active={active} label="Scaffold"  onClick={onChange}
      icon={<svg viewBox="0 0 16 16" fill="currentColor"><path d="M6 2L2 6l1 1 1-1v6h1V6l1 1 1-1zm4 0l-1 1 1 1V2zm0 0v4l1-1 1 1 1-1-4-4zm0 12l4-4-1-1-1 1V6h-1v6l-1-1-1 1z"/></svg>}
    />
    <ActivityItem section="endpoints" active={active} label="Endpoints" onClick={onChange}
      icon={<svg viewBox="0 0 16 16" fill="currentColor"><path d="M5 3a2 2 0 1 0 0 4 2 2 0 0 0 0-4zm0 1a1 1 0 1 1 0 2 1 1 0 0 1 0-2zm6 5a2 2 0 1 0 0 4 2 2 0 0 0 0-4zm0 1a1 1 0 1 1 0 2 1 1 0 0 1 0-2zM4 9h1v1H4zm1 0h6v1H5zm5 0h1v1h-1z"/></svg>}
    />
    <ActivityItem section="setup"     active={active} label="Ambiente"  onClick={onChange}
      icon={<svg viewBox="0 0 16 16" fill="currentColor"><path d="M9.1 4.4L8.6 2H7.4l-.5 2.4-.7.3-2-1.3-.9.8 1.3 2-.2.7-2.4.5v1.2l2.4.5.3.7-1.3 2 .8.8 2-1.3.7.3.5 2.4h1.2l.5-2.4.7-.3 2 1.3.8-.8-1.3-2 .3-.7 2.4-.5V7.4l-2.4-.5-.3-.7 1.3-2-.8-.8-2 1.3-.7-.3zM8 10a2 2 0 1 1 0-4 2 2 0 0 1 0 4z"/></svg>}
    />
    <div className="activity-spacer" />
    <div className="activity-bottom">
      <div className="activity-item" onClick={onSwitch} role="button" aria-label="Trocar projeto">
        <svg viewBox="0 0 16 16" fill="currentColor"><path d="M8 1a7 7 0 1 0 0 14A7 7 0 0 0 8 1zm0 1a6 6 0 1 1 0 12A6 6 0 0 1 8 2zm-.5 3v2.5H5l3 3 3-3H8.5V5h-1z"/></svg>
        <span className="activity-tooltip">Trocar projeto</span>
      </div>
    </div>
  </aside>
)
