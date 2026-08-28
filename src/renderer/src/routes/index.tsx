import React from 'react'
import { Routes, Route, Navigate } from 'react-router-dom'
import { MainLayout } from '../components/layout/MainLayout'
import { HomePage } from '../pages/Home'
import { SetupPage } from '../pages/Setup'
import { ScaffoldPage } from '../pages/Scaffold'
import { EndpointsPage } from '../pages/Endpoints'

interface AppRoutesProps {
  projectPath: string
  onClearProject: () => void
}

export const AppRoutes: React.FC<AppRoutesProps> = ({ projectPath, onClearProject }) => (
  <Routes>
    <Route element={<MainLayout projectPath={projectPath} onClearProject={onClearProject} />}>
      <Route index element={<Navigate to="/home" replace />} />
      <Route path="/home"      element={<HomePage />} />
      <Route path="/setup"     element={<SetupPage />} />
      <Route path="/scaffold"  element={<ScaffoldPage />} />
      <Route path="/endpoints" element={<EndpointsPage />} />
    </Route>
  </Routes>
)
