cmake_minimum_required(VERSION 3.16)
project(scaffold_zeus CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(scaffold_zeus
    main.cpp
    application/ScaffoldUseCase.cpp
    application/EndpointUseCase.cpp
    application/EnvironmentUseCase.cpp
    infra/templates/PhpTemplates.cpp
    infra/scaffold/BackendScaffold.cpp
    infra/scaffold/FrontendScaffold.cpp
    infra/scaffold/ElectronScaffold.cpp
    infra/environment/Environment.cpp
)

target_include_directories(scaffold_zeus PRIVATE .)
