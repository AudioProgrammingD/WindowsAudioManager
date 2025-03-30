cmake_minimum_required(VERSION 3.16)
project(AudioSwitcher LANGUAGES CXX)

# Set the C++ standard to C++17.
set(CMAKE_CXX_STANDARD 17)

# Set output directories:
# - bin/:       Executables and shared libraries (.dll)
# - lib/:       Static libraries (.lib) and import libraries (for .dll)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# ----------------------------------------------------------------------------
# COMMON SOURCES (used by both static and shared)
# ----------------------------------------------------------------------------
set(AUDIO_SWITCHER_SOURCES
    src/AudioSwitcher/AudioSwitcher.cpp
    src/Utility/DeviceUtils.cpp
    src/Utility/COMInitializer.cpp
)

# ----------------------------------------------------------------------------
# STATIC LIBRARY: AudioSwitcherStatic
# ----------------------------------------------------------------------------
# For the static library, define AUDIO_SWITCHER_STATIC so our export macro 
# expands to nothing in the headers. This ensures no __declspec(dllimport) 
# is applied when using this library in the test executable.
add_library(AudioSwitcherStatic STATIC
    ${AUDIO_SWITCHER_SOURCES}
)

# Provide include paths for our public headers.
target_include_directories(AudioSwitcherStatic PUBLIC ${CMAKE_SOURCE_DIR}/include)

# Define AUDIO_SWITCHER_STATIC as PUBLIC so any target linking this library sees it too.
# This ensures that in your test/main.cpp (or any consumer), the macro expands to nothing.
# We also define _UNICODE and UNICODE for the static build.
target_compile_definitions(AudioSwitcherStatic
    PUBLIC AUDIO_SWITCHER_STATIC
    PRIVATE _UNICODE
    PRIVATE UNICODE
)

# Link against Windows libraries needed for COM, etc.
target_link_libraries(AudioSwitcherStatic ole32 uuid)

# ----------------------------------------------------------------------------
# SHARED LIBRARY: AudioSwitcherShared
# ----------------------------------------------------------------------------
# For the shared library, we add a dummy source file that forces symbol export 
# so the linker creates an import library (.lib) alongside the .dll.
set(AUDIO_SWITCHER_SHARED_SOURCES
    ${AUDIO_SWITCHER_SOURCES}
    src/AudioSwitcher/AudioSwitcherDummy.cpp  # Dummy function to force an export
)

add_library(AudioSwitcherShared SHARED ${AUDIO_SWITCHER_SHARED_SOURCES})
target_include_directories(AudioSwitcherShared PUBLIC ${CMAKE_SOURCE_DIR}/include)

# Define AUDIO_SWITCHER_EXPORTS so that AUDIO_SWITCHER_API becomes __declspec(dllexport).
# This ensures the DLL properly exports symbols.
target_compile_definitions(AudioSwitcherShared
    PRIVATE AUDIO_SWITCHER_EXPORTS
    PRIVATE _UNICODE
    PRIVATE UNICODE
)

target_link_libraries(AudioSwitcherShared ole32 uuid)

# ----------------------------------------------------------------------------
# TEST EXECUTABLE: AudioSwitcherTest
# ----------------------------------------------------------------------------
# Create a test executable that links against the AudioSwitcher library.
# By default, we link the test with the STATIC library (AudioSwitcherStatic).
add_executable(AudioSwitcherTest test/main.cpp)
target_link_libraries(AudioSwitcherTest AudioSwitcherStatic)



# === STATIC TEST ===
add_executable(AudioSwitcherTest_Static test/main.cpp)
target_compile_definitions(AudioSwitcherTest_Static PRIVATE AUDIO_SWITCHER_STATIC)
target_link_libraries(AudioSwitcherTest_Static PRIVATE AudioSwitcherStatic)


# === SHARED TEST ===
add_executable(AudioSwitcherTest_Shared test/main.cpp)
target_link_libraries(AudioSwitcherTest_Shared PRIVATE AudioSwitcherShared)

# Copy DLL next to the shared test executable on Windows
if (WIN32)
    add_custom_command(TARGET AudioSwitcherTest_Shared POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_SOURCE_DIR}/bin/$<CONFIG>/AudioSwitcherShared.dll"
            "$<TARGET_FILE_DIR:AudioSwitcherTest_Shared>"
    )
endif()

# ----------------------------------------------------------------------------
# USAGE & NOTES:
#
# 1. Static Library (AudioSwitcherStatic):
#    - The macro AUDIO_SWITCHER_STATIC is defined publicly,
#      so headers expand to no import/export (i.e., no dllimport/dllexport).
#    - Produces a .lib in /lib/ (no .dll needed).
#
# 2. Shared Library (AudioSwitcherShared):
#    - The macro AUDIO_SWITCHER_EXPORTS is defined, so headers expand to __declspec(dllexport).
#    - Dummy export function in AudioSwitcherDummy.cpp ensures at least one symbol is exported,
#      producing AudioSwitcherShared.lib (import library) and AudioSwitcherShared.dll in /bin/.
#
# 3. Test Executable:
#    - Currently links against AudioSwitcherStatic (no runtime .dll needed).
#    - If you want to test the shared library, change the line:
#         target_link_libraries(AudioSwitcherTest AudioSwitcherStatic)
#      to:
#         target_link_libraries(AudioSwitcherTest AudioSwitcherShared)
#      and ensure AudioSwitcherShared.dll is in the same folder at runtime.
#
# 4. Folder Structure:
#    - include/AudioSwitcher/AudioSwitcher.h :  Public header with macro logic
#    - src/AudioSwitcher/AudioSwitcher.cpp :    Main API code
#    - src/AudioSwitcher/AudioSwitcherDummy.cpp :  Dummy export function for the DLL
#    - src/Utility/DeviceUtils.cpp :           Additional utility code
#    - test/main.cpp :                         Test application
#
# 5. Integration:
#    - Option 1: install() + find_package()
#    - Option 2: add_subdirectory(path/to/AudioSwitcher) in a parent project.
# ----------------------------------------------------------------------------
