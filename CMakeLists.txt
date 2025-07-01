cmake_minimum_required(VERSION 3.28.3)
project(Abyss)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

if (MSVC)
    set(COMPILE_OPTS 
        /W4 
        /wd4100 
        /wd4201 
        /wd4505 
        /Zc:preprocessor
    )
    set(LINK_OPTS /SUBSYSTEM:WINDOWS)
else()
    set(COMPILE_OPTS 
        -Wall -Wextra 
        -Wno-unused-parameter 
        -Wno-ignored-qualifiers 
        -Wno-unused-function
        -Wno-gnu-anonymous-struct 
        -Wno-nested-anon-types
    )
    set(LINK_OPTS)
endif()

# Check if we're building ImGui internally
if (NOT DEFINED IMGUI_DIR OR NOT DEFINED IMGUI_INCLUDE_DIR)
    set(STANDALONE TRUE)
else()
    set(STANDALONE FALSE)
endif()

if (STANDALONE)
    message(STATUS "Building internal ImGui")
    set(IMGUI_DIR "Vendor/ImGui")
    set(IMGUI_INCLUDE_DIR "Vendor/ImGui")

    set(IMGUI_HEADERS
        ${IMGUI_DIR}/imgui_internal.h
        ${IMGUI_DIR}/imgui.h
        ${IMGUI_DIR}/imstb_rectpack.h
        ${IMGUI_DIR}/imstb_textedit.h
        ${IMGUI_DIR}/imstb_truetype.h
    )

    set(IMGUI_SOURCES
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
        ${IMGUI_DIR}/imgui_demo.cpp
        ${IMGUI_DIR}/imgui.cpp
    )
    
    add_library(ImGui STATIC ${IMGUI_HEADERS} ${IMGUI_SOURCES} ${IMGUI_BACKENDS})

    target_include_directories(ImGui PUBLIC
        ${IMGUI_DIR}
        ${IMGUI_INCLUDE_DIR}
        ${VULKAN_INCLUDE_DIR}
        ${GLFW_INCLUDE_DIR}
        ${GLM_INCLUDE_DIR}
        "Source/Public"
    )

    # target_compile_definitions(ImGui PUBLIC IMGUI_USER_CONFIG="Platform/imgui/imconfig.h")
    target_compile_options(ImGui PUBLIC ${COMPILE_OPTS})

    set_target_properties(ImGui PROPERTIES FOLDER "Dependencies/ImGui")
else()
    message(STATUS "Using external ImGui target")
    if (NOT TARGET ImGui)
        message(FATAL_ERROR "Expected ImGui target to exist when STANDALONE is OFF")
    endif()
endif()

# Setup Litehtml
add_subdirectory(Vendor/litehtml)
set_target_properties(litehtml PROPERTIES FOLDER "Dependencies/litehtml")

# Sources

set(CPP_SOURCES
    Source/Private/Browser.cpp
)

set(CPP_HEADERS
    Source/Public/ImWeb/Browser.h
)

# Build

add_library(AbyssImWeb ${CPP_SOURCES} ${CPP_HEADERS})

target_link_libraries(AbyssImWeb PUBLIC ImGui litehtml)

target_include_directories(AbyssImWeb PUBLIC
    "Source/Public"
    ${IMGUI_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Vendor/litehtml/include    
)

target_compile_options(AbyssImWeb PUBLIC ${COMPILE_OPTS})
