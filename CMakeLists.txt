cmake_minimum_required(VERSION 3.20)
project(BzWare VERSION 4.2.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ── Flags MSVC ──────────────────────────────────────────────────────────────
if(MSVC)
    # /O2  = otimização máxima
    # /MT  = CRT estático (sem dependência de DLLs do runtime)
    # /W3  = avisos moderados
    # /EHsc = exception handling padrão
    # /SUBSYSTEM:WINDOWS = sem console na release (WinMain)
    add_compile_options(/O2 /W3 /EHsc /MT)
    add_link_options(/SUBSYSTEM:WINDOWS)
endif()

# ── Executável ──────────────────────────────────────────────────────────────
add_executable(bzware WIN32
    src/main.cpp
)

# ── Bibliotecas Windows necessárias ─────────────────────────────────────────
target_link_libraries(bzware PRIVATE
    user32      # janelas, mensagens, input
    gdi32       # GDI (drawing)
    shell32     # ShellExecuteA
    kernel32    # AllocConsole, GetStdHandle
)

# ── Output ──────────────────────────────────────────────────────────────────
set_target_properties(bzware PROPERTIES
    OUTPUT_NAME "bzware"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Release"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/Release"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_BINARY_DIR}/Debug"
)
