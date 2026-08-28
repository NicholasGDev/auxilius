#!/usr/bin/env bash
# Run the full Auxilius stack via Docker (C++ build + Electron app via X11)
set -euo pipefail

echo "Allowing X11 connections from Docker..."
xhost +local:docker 2>/dev/null || true

echo "Building and running via Docker Compose..."
docker compose up --build app-dev
