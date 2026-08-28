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

# Stage 2: dev image — runs Electron via X11 forwarding
FROM node:20-slim AS app-dev

RUN apt-get update && apt-get install -y --no-install-recommends \
    # Electron runtime deps
    libnss3 libatk1.0-0 libatk-bridge2.0-0 \
    libcups2 libdrm2 libxkbcommon0 libxcomposite1 \
    libxdamage1 libxfixes3 libxrandr2 libgbm1 \
    libasound2t64 libx11-6 libx11-xcb1 libxcb1 \
    libxext6 libxi6 libxrender1 libxtst6 \
    ca-certificates fonts-liberation \
    # C++ compiler for in-app compile feature
    g++ \
 && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Install deps in a separate layer for better cache
COPY package.json package-lock.json* ./
RUN npm install

COPY . .
COPY --from=cpp-builder /build/scaffold_zeus ./bin/scaffold_zeus

CMD ["npm", "run", "dev"]

# Stage 3: production build
FROM node:20-slim AS app-builder

WORKDIR /app
COPY package.json package-lock.json* ./
RUN npm ci --prefer-offline 2>/dev/null || npm install

COPY . .
COPY --from=cpp-builder /build/scaffold_zeus ./bin/scaffold_zeus

RUN npm run build

# Stage 4: final artifacts only
FROM scratch AS artifacts
COPY --from=cpp-builder /build/scaffold_zeus /bin/scaffold_zeus
COPY --from=app-builder /app/out /out
