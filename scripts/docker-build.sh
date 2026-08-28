#!/usr/bin/env bash
# Build C++ binary via Docker and copy to bin/
set -euo pipefail

echo "Building C++ binary via Docker..."
mkdir -p bin
docker compose run --rm cpp-build
echo "Done. Binary at: bin/scaffold_zeus"
