import React from 'react'
import ReactDOM from 'react-dom/client'
import '@giro-ds/tokens/build/css/tokens.css'
import '@giro-ds/react/dist/styles.css'
import './styles/global.scss'
import App from './App'

ReactDOM.createRoot(document.getElementById('root')!).render(
  <React.StrictMode>
    <App />
  </React.StrictMode>
)
