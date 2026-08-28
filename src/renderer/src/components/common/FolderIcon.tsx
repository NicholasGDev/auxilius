import React from 'react'

interface Props {
  open?: boolean
  size?: number
  className?: string
}

export const FolderIcon: React.FC<Props> = ({ open = false, size = 16, className }) => (
  <svg
    width={size}
    height={size}
    viewBox="0 0 24 24"
    fill="none"
    xmlns="http://www.w3.org/2000/svg"
    className={className}
    style={{ flexShrink: 0 }}
  >
    {open ? (
      <>
        <path d="M2 6C2 4.9 2.9 4 4 4H9L11 6H20C21.1 6 22 6.9 22 8V9H4C2.9 9 2 8.1 2 7V6Z" fill="#D4A017" />
        <path d="M2 9H22L20 19C19.8 19.6 19.2 20 18.6 20H5.4C4.8 20 4.2 19.6 4 19L2 9Z" fill="#F5C518" />
      </>
    ) : (
      <>
        <path d="M4 4C2.9 4 2 4.9 2 6V18C2 19.1 2.9 20 4 20H20C21.1 20 22 19.1 22 18V8C22 6.9 21.1 6 20 6H11L9 4H4Z" fill="#F5C518" />
        <path d="M4 4H9L11 6H20C21.1 6 22 6.9 22 8H2V6C2 4.9 2.9 4 4 4Z" fill="#D4A017" />
      </>
    )}
  </svg>
)
