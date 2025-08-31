# IGED UE5 - CMake Build Configuration
# Copyright (c) 2024 Project Human Bot

cmake_minimum_required(VERSION 3.20)
project(IGED_UE5 VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Configure build types
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
endif()

# Platform detection
if(WIN32)
    add_definitions(-DPLATFORM_WINDOWS=1)
    set(PLATFORM_NAME "Win64")
elseif(UNIX AND NOT APPLE)
    add_definitions(-DPLATFORM_LINUX=1)
    set(PLATFORM_NAME "Linux")
else()
    message(FATAL_ERROR "Unsupported platform")
endif()

# Unreal Engine 5.4 paths
set(UE5_ROOT "$ENV{UE5_ROOT}" CACHE PATH "Path to Unreal Engine 5.4")
if(NOT EXISTS "${UE5_ROOT}")
    set(UE5_ROOT "C:/Program Files/Epic Games/UE_5.4")
endif()

set(UE5_ENGINE_DIR "${UE5_ROOT}/Engine")
set(UE5_BINARIES_DIR "${UE5_ENGINE_DIR}/Binaries/${PLATFORM_NAME}")
set(UE5_INTERMEDIATE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Intermediate")
set(UE5_PROJECT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

# Include directories
include_directories(
    "${UE5_ENGINE_DIR}/Source/Runtime/Core/Public"
    "${UE5_ENGINE_DIR}/Source/Runtime/CoreUObject/Public"
    "${UE5_ENGINE_DIR}/Source/Runtime/Engine/Public"
    "${UE5_ENGINE_DIR}/Source/Runtime/InputCore/Public"
    "${UE5_ENGINE_DIR}/Source/Runtime/Json/Public"
    "${UE5_ENGINE_DIR}/Source/Runtime/HTTP/Public"
    "${UE5_ENGINE_DIR}/Source/Runtime/UMG/Public"
    "${UE5_ENGINE_DIR}/Source/Runtime/Slate/Public"
    "${UE5_ENGINE_DIR}/Source/Runtime/SlateCore/Public"
    "${UE5_ENGINE_DIR}/Source/Runtime/Niagara/Public"
    "${UE5_ENGINE_DIR}/Source/Runtime/NiagaraCore/Public"
    "${UE5_ENGINE_DIR}/Source/Runtime/AudioCapture/Public"
    "${UE5_ENGINE_DIR}/Source/Developer/ToolMenus/Public"
    "${UE5_ENGINE_DIR}/Plugins/EnhancedInput/Source/EnhancedInput/Public"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/IGED_UE5/Public"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/IGED_UE5/Public/Core"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/IGED_UE5/Public/Agents"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/IGED_UE5/Public/UI"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/IGED_UE5/Public/Security"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/IGED_UE5/Public/Networking"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/IGED_UE5/Public/Visualization"
)

# Collect source files
file(GLOB_RECURSE IGED_SOURCE_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/IGED_UE5/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/IGED_UE5/*.h"
)

# Create the main module library
add_library(IGED_UE5 SHARED ${IGED_SOURCE_FILES})

# Set properties
set_target_properties(IGED_UE5 PROPERTIES
    OUTPUT_NAME "IGED_UE5"
    PREFIX ""
    SUFFIX ".dll"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Binaries/${PLATFORM_NAME}"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Binaries/${PLATFORM_NAME}"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Intermediate/${PLATFORM_NAME}"
)

# Compiler flags for Windows
if(WIN32)
    target_compile_options(IGED_UE5 PRIVATE
        /W4                     # Warning level 4
        /WX-                    # Warnings as errors disabled
        /MP                     # Multi-processor compilation
        /std:c++20              # C++20 standard
        /permissive-            # Conformance mode
        /Zc:inline              # Remove unreferenced functions
        /Zc:strictStrings       # Strict string literals
        /Zc:rvalueCast          # Strict rvalue cast
        /fp:fast                # Fast floating point
        /GR                     # RTTI enabled
        /EHsc                   # Exception handling
        /MD                     # Multi-threaded DLL runtime
        /bigobj                 # Large object files
        /Oi                     # Intrinsic functions
        /Ot                     # Favor speed
        /GL                     # Whole program optimization
    )

    # Link flags
    target_link_options(IGED_UE5 PRIVATE
        /LTCG                   # Link time code generation
        /OPT:REF                # Eliminate unreferenced functions
        /OPT:ICF                # Identical COMDAT folding
        /MACHINE:X64            # Target x64
        /SUBSYSTEM:WINDOWS      # Windows subsystem
        /DYNAMICBASE            # ASLR
        /NXCOMPAT               # DEP compatible
        /LARGEADDRESSAWARE      # Large address aware
    )
endif()

# Link UE5 libraries
if(WIN32)
    target_link_libraries(IGED_UE5 PRIVATE
        "${UE5_ENGINE_DIR}/Intermediate/Build/Win64/UnrealEditor/Development/Core/UnrealEditor-Core.lib"
        "${UE5_ENGINE_DIR}/Intermediate/Build/Win64/UnrealEditor/Development/CoreUObject/UnrealEditor-CoreUObject.lib"
        "${UE5_ENGINE_DIR}/Intermediate/Build/Win64/UnrealEditor/Development/Engine/UnrealEditor-Engine.lib"
        "${UE5_ENGINE_DIR}/Intermediate/Build/Win64/UnrealEditor/Development/InputCore/UnrealEditor-InputCore.lib"
        "${UE5_ENGINE_DIR}/Intermediate/Build/Win64/UnrealEditor/Development/Json/UnrealEditor-Json.lib"
        "${UE5_ENGINE_DIR}/Intermediate/Build/Win64/UnrealEditor/Development/HTTP/UnrealEditor-HTTP.lib"
        "${UE5_ENGINE_DIR}/Intermediate/Build/Win64/UnrealEditor/Development/UMG/UnrealEditor-UMG.lib"
        "${UE5_ENGINE_DIR}/Intermediate/Build/Win64/UnrealEditor/Development/Slate/UnrealEditor-Slate.lib"
        "${UE5_ENGINE_DIR}/Intermediate/Build/Win64/UnrealEditor/Development/SlateCore/UnrealEditor-SlateCore.lib"
        "${UE5_ENGINE_DIR}/Intermediate/Build/Win64/UnrealEditor/Development/Niagara/UnrealEditor-Niagara.lib"
        "${UE5_ENGINE_DIR}/Intermediate/Build/Win64/UnrealEditor/Development/AudioCapture/UnrealEditor-AudioCapture.lib"
    )

    # Windows system libraries
    target_link_libraries(IGED_UE5 PRIVATE
        winhttp
        bcrypt
        ncrypt
        crypt32
        secur32
        userenv
        ws2_32
        dbghelp
        shlwapi
        kernel32
        user32
        gdi32
        winspool
        comdlg32
        advapi32
        shell32
        ole32
        oleaut32
        uuid
        odbc32
        odbccp32
    )
endif()

# Custom build commands
add_custom_command(TARGET IGED_UE5 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "✅ IGED_UE5 built successfully!"
    COMMAND ${CMAKE_COMMAND} -E echo "📁 Output: $<TARGET_FILE:IGED_UE5>"
)

# Generate Visual Studio filters
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${IGED_SOURCE_FILES})

# Package configuration
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_custom_target(Package
        COMMAND ${CMAKE_COMMAND} -E echo "📦 Packaging IGED_UE5 for distribution..."
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_SOURCE_DIR}/Package"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/Binaries" "${CMAKE_CURRENT_SOURCE_DIR}/Package/Binaries"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/Content" "${CMAKE_CURRENT_SOURCE_DIR}/Package/Content"
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/IGED_UE5.uproject" "${CMAKE_CURRENT_SOURCE_DIR}/Package/"
        DEPENDS IGED_UE5
    )
endif()

# Installation rules
install(TARGETS IGED_UE5
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Content"
    DESTINATION .
    PATTERN "*.uasset"
    PATTERN "*.umap"
)

install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/IGED_UE5.uproject"
    DESTINATION .
)

# CPack configuration for installer
set(CPACK_PACKAGE_NAME "IGED_UE5")
set(CPACK_PACKAGE_VENDOR "Project Human Bot")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "IGED - Advanced AI Assistant with UE5")
set(CPACK_PACKAGE_VERSION_MAJOR 1)
set(CPACK_PACKAGE_VERSION_MINOR 0)
set(CPACK_PACKAGE_VERSION_PATCH 0)
set(CPACK_PACKAGE_INSTALL_DIRECTORY "IGED_UE5")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")

if(WIN32)
    set(CPACK_GENERATOR "NSIS;ZIP")
    set(CPACK_NSIS_DISPLAY_NAME "IGED UE5")
    set(CPACK_NSIS_HELP_LINK "https://github.com/project-human-bot/IGED")
    set(CPACK_NSIS_URL_INFO_ABOUT "https://github.com/project-human-bot/IGED")
    set(CPACK_NSIS_CONTACT "support@projecthumanbot.ai")
    set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
    set(CPACK_NSIS_MODIFY_PATH ON)
endif()

include(CPack)
