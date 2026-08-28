# Stage 1: compile the C++ binary
FROM gcc:12 AS cpp-builder

WORKDIR /build
COPY cpp/ .

RUN g++ -std=c++20 -O2 \
    main.cpp \
    application/ScaffoldUseCase.cpp \
    application/EndpointUseCase.cpp \
    application/EnvironmentUseCase.cpp \
    infra/templates/PhpTemplates.cpp \
    infra/scaffold/BackendScaffold.cpp \
    infra/scaffold/FrontendScaffold.cpp \
    infra/scaffold/ElectronScaffold.cpp \
    infra/environment/Environment.cpp \
    -o scaffold_zeus

# Stage 2: build the Electron/Node app (no display needed — just bundles assets)
FROM node:20-slim AS app-builder

WORKDIR /app
COPY package.json package-lock.json* ./
RUN npm ci --prefer-offline 2>/dev/null || npm install

COPY . .
# Bring in the compiled binary
COPY --from=cpp-builder /build/scaffold_zeus ./bin/scaffold_zeus

RUN npm run build

# Final image — just the built artifacts + binary
FROM scratch AS artifacts
COPY --from=cpp-builder /build/scaffold_zeus /bin/scaffold_zeus
COPY --from=app-builder /app/out /out
