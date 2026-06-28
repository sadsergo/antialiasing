cmake_minimum_required(VERSION 3.20)
project(antialiasing CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ── Slang ─────────────────────────────────────────────────────────────────────
set(SLANG_ROOT "/home/nikit/slang" CACHE PATH "Slang installation root")
find_program(SLANGC "${SLANG_ROOT}/bin/slangc" REQUIRED)

# ── GLM ───────────────────────────────────────────────────────────────────────
find_package(glm REQUIRED)

# ── Generate C++ from Slang shaders at build time ────────────────────────────
set(SHADER_DIR "${CMAKE_SOURCE_DIR}/shaders")
set(GEN_DIR    "${CMAKE_BINARY_DIR}/gen")
file(MAKE_DIRECTORY "${GEN_DIR}")

add_custom_command(
    OUTPUT  "${GEN_DIR}/vertex_gen.cpp"
    COMMAND "${SLANGC}"
            "${SHADER_DIR}/vertex.slang"
            -target cpp
            -entry  vertexMain
            -o      "${GEN_DIR}/vertex_gen.cpp"
    DEPENDS "${SHADER_DIR}/vertex.slang"
    COMMENT "Compiling vertex.slang → C++"
)

add_custom_command(
    OUTPUT  "${GEN_DIR}/fragment_gen.cpp"
    COMMAND "${SLANGC}"
            "${SHADER_DIR}/fragment.slang"
            -target cpp
            -entry  fragmentMain
            -o      "${GEN_DIR}/fragment_gen.cpp"
    DEPENDS "${SHADER_DIR}/fragment.slang"
    COMMENT "Compiling fragment.slang → C++"
)

# ── Executable ────────────────────────────────────────────────────────────────
add_executable(antialiasing
    src/main.cpp
    src/image.cpp
    src/rasterizer.cpp
    src/shader_dispatch.cpp
    "${GEN_DIR}/vertex_gen.cpp"
    "${GEN_DIR}/fragment_gen.cpp"
)

target_include_directories(antialiasing PRIVATE
    src/
    "${SLANG_ROOT}/include"
)

target_link_libraries(antialiasing PRIVATE glm::glm)

# Suppress warnings from Slang-generated code
set_source_files_properties(
    "${GEN_DIR}/vertex_gen.cpp"
    "${GEN_DIR}/fragment_gen.cpp"
    PROPERTIES COMPILE_FLAGS "-w"
)
