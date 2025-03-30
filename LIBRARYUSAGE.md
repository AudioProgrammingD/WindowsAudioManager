# 📦 LIBRARYUSAGE.md — Using WinAudioManager in Your Project

**WinAudioManager** is a modern, modular C++ library for managing audio playback devices on Windows using the Core Audio COM APIs.

You can easily integrate it as either a:

- **Static Library** (`.lib`) — embed everything into your executable
- **Shared Library** (`.dll + .lib`) — load and link the exported functions dynamically

> ⚠️ This library internally uses the undocumented `IPolicyConfig` COM interface to switch audio devices. It's recommended to use this as a **modular, plug-and-play component** so it can be detached or patched without affecting the rest of your application.

---

## 📁 Folder Layout (after build)

```bash
WinAudioManager/
├── include/                     ← Public headers to copy into your project
├── lib/
│   ├── AudioSwitcherStatic.lib  ← Static library
│   ├── AudioSwitcherShared.lib  ← Import library for the DLL
├── bin/
│   └── AudioSwitcherShared.dll  ← Shared library (DLL)
```

---

## 🧱 Option 1: Use the Static Library

### ✅ Folder Setup (Your Project)

```bash
YourApp/
├── include/                    ← Copy from WinAudioManager/include/
├── lib/                        ← Copy AudioSwitcherStatic.lib
├── src/
│   └── main.cpp
├── CMakeLists.txt
```

### ✅ CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.16)
project(UseAudioStatic)

set(CMAKE_CXX_STANDARD 17)

include_directories(${CMAKE_SOURCE_DIR}/include)
link_directories(${CMAKE_SOURCE_DIR}/lib)

add_executable(MyApp src/main.cpp)

# Use static version: disables __declspec(dllexport/dllimport)
target_compile_definitions(MyApp PRIVATE AUDIO_SWITCHER_STATIC)

target_link_libraries(MyApp PRIVATE AudioSwitcherStatic)
```

---

## 🔄 Option 2: Use the Shared Library (DLL)

### ✅ Folder Setup

```bash
YourApp/
├── include/                       ← Copy from WinAudioManager/include/
├── lib/
│   ├── AudioSwitcherShared.lib    ← Import library
│   └── AudioSwitcherShared.dll    ← Shared DLL
├── src/
│   └── main.cpp
├── CMakeLists.txt
```

### ✅CMAKE:CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.16)
project(UseAudioShared)

set(CMAKE_CXX_STANDARD 17)

include_directories(${CMAKE_SOURCE_DIR}/include)
link_directories(${CMAKE_SOURCE_DIR}/lib)

add_executable(MyApp_Shared src/main.cpp)
target_link_libraries(MyApp_Shared PRIVATE AudioSwitcherShared)

# Copy the DLL next to the .exe for runtime (Windows only)
if (WIN32)
    add_custom_command(TARGET MyApp_Shared POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_SOURCE_DIR}/lib/AudioSwitcherShared.dll"
            "$<TARGET_FILE_DIR:MyApp_Shared>"
    )
endif()
```

---

## ⚙️ Macro Behavior

Headers automatically use the correct decoration depending on build context:

```cpp
#if defined(_WIN32)
    #if defined(AUDIO_SWITCHER_STATIC)
        #define AUDIO_SWITCHER_API
    #elif defined(AUDIO_SWITCHER_EXPORTS)
        #define AUDIO_SWITCHER_API __declspec(dllexport)
    #else
        #define AUDIO_SWITCHER_API __declspec(dllimport)
    #endif
#else
    #define AUDIO_SWITCHER_API
#endif
```

| Use Case                   | Define This Macro           | Result                      |
|----------------------------|-----------------------------|-----------------------------|
| Building static lib        | `AUDIO_SWITCHER_STATIC`     | No export/import decoration |
| Building shared lib (DLL)  | `AUDIO_SWITCHER_EXPORTS`    | Export symbols              |
| Using shared lib (DLL)     | *Nothing (default)*         | Import symbols              |

---

## 🧪 Example Usage

```cpp
#include "AudioSwitcher/AudioSwitcher.h"
#include "Utility/DeviceUtils.h"
#include "Utility/COMInitializer.h"
#include "Utility/SafeRelease.h"

using namespace AudioSwitcher;
using namespace Utility;

int main() {
    COMInitializer com;

    auto devices = AudioManager::listOutputDevices();
    for (size_t i = 0; i < devices.size(); ++i)
        std::wcout << L"[" << i << L"] " << devices[i].name << L"\n";

    if (!devices.empty()) {
        AudioManager::setDefaultOutputDevice(devices[0].id); // Set as default
    }

    IMMDevice* def = GetDefaultAudioPlaybackDevice();
    if (def) {
        MuteDevice(def, true); // Mute the default device
        SafeRelease(def);
    }

    return 0;
}
```

---

## 📎 Tip: Clean Modular Design

This library is intentionally structured as an independent module that can be:

- 🔌 Plugged into your tool or engine
- 🔧 Easily detached or replaced (if IPolicyConfig breaks)
- 📁 Reused across multiple projects

You can copy only the `include/` folder and either:

- `lib/AudioSwitcherStatic.lib` (static)
- or `lib/AudioSwitcherShared.lib` + `bin/AudioSwitcherShared.dll` (shared)

📍 It's up to your project whether to embed it (static) or keep it modular (shared).

---

## 🔗 See Also

- 📖 [README.md](./README.md) — Library overview, features, and API reference
- 🧠 [IPolicyConfig Header Source](https://github.com/tartakynov/audioswitch/blob/master/IPolicyConfig.h)

---

## 🙌 Credits

Developed on top of Windows Core Audio COM APIs  
Special thanks to the original author of `IPolicyConfig.h`  
→ [tartakynov/audioswitch](https://github.com/tartakynov/audioswitch)
