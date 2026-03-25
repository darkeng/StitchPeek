cmake_minimum_required(VERSION 3.15)
project(StitchPeek VERSION 1.0.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Statically link CRT for simpler deployment (apply to all targets including libembroidery)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

# Add libembroidery
# We don't want to build its installer/packaging logic
set(CPACK_GENERATOR "")
add_subdirectory(libembroidery)

# Build the DLL
add_library(StitchPeek SHARED
    src/dllmain.cpp
    src/StitchPeek.cpp
    src/StitchRenderer.cpp
    src/Registry.cpp
    src/StitchPeek.def
)

target_include_directories(StitchPeek PRIVATE src libembroidery)
# Require dependencies for COM, GDI+, WIC, Shlwapi
target_link_libraries(StitchPeek PRIVATE 
    embroidery_static
    gdiplus
    shlwapi
    propsys
    windowscodecs
    uuid
    ole32
    oleaut32
    gdi32
    user32
)

add_executable(test_thumbnail tools/test_thumbnail.cpp src/StitchRenderer.cpp)
target_include_directories(test_thumbnail PRIVATE src libembroidery)
target_link_libraries(test_thumbnail PRIVATE embroidery_static gdiplus)

# Definitions
target_compile_definitions(StitchPeek PRIVATE _USRDLL _WINDLL UNICODE _UNICODE)
