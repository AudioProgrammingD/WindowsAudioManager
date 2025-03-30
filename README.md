# 🎧 WinAudioManager

**WinAudioManager** is a lightweight C++17 library for controlling audio output devices on **Windows**.

It wraps the Windows Core Audio APIs (COM-based) to provide a simple, modern, and safe interface for:

- 🔎 Enumerating audio devices
- ✅ Switching the system default output device
- 🔇 Muting specific or default devices
- 📊 Retrieving detailed audio format metadata

Built with **RAII-compliance**, **static/dynamic library support**, and modularity in mind — perfect for integrating into modern C++ applications or tools.

---

## 🚨 Disclaimer

⚠️ This library uses an **undocumented Windows COM interface** (`IPolicyConfig`) to switch the default audio device.  
While widely used and stable, it is **not officially supported by Microsoft** and may break in future Windows updates.

**Design Tip**: WinAudioManager is built as a standalone module — if any API changes or breakage occurs, it can be easily removed, patched, or swapped without tightly coupling with the rest of your system.

🔗 `IPolicyConfig.h` was sourced from this project:  
👉 [tartakynov/audioswitch (GitHub)](https://github.com/tartakynov/audioswitch/blob/master/IPolicyConfig.h)  
All credit to the original author for documenting and exposing this functionality 🙌

---

## 📚 Library Usage & Integration

📖 See [📄 LIBRARYUSAGE.md](./LIBRARYUSAGE.md) for:

- Building the static and shared libraries
- Linking into your own project
- Directory structure
- Using macros like `AUDIO_SWITCHER_STATIC`
- Full CMake usage example

---

## ✨ Features

- ✅ Set default playback device via Device ID
- 🔎 List all active audio output devices (IMMDevice)
- 📛 Retrieve friendly device name
- 🎚️ Access audio format metadata (bit depth, channels, sample rate)
- 🔇 Mute/unmute default or specific device
- 🛠️ RAII-style `COMInitializer` helper
- 🔄 Build as either static `.lib` or shared `.dll + .lib`

---

## 📁 Project Structure

```bash
WinAudioManager/
├── include/
│   ├── AudioSwitcher/AudioSwitcher.h
│   └── Utility/
│       ├── COMInitializer.h
│       ├── DeviceUtils.h
│       ├── DeviceFormatInfo.h
│       └── SafeRelease.h
├── src/
│   ├── AudioSwitcher/AudioSwitcher.cpp
│   ├── AudioSwitcher/AudioSwitcherDummy.cpp
│   └── Utility/
│       ├── COMInitializer.cpp
│       └── DeviceUtils.cpp
├── test/
│   └── main.cpp
├── bin/         # Built DLLs and test executables
├── lib/         # Static and import libraries
└── CMakeLists.txt
```

---

## ⚙️ Build Instructions

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```

- 📦 `lib/` → Static `.lib` and shared import libraries  
- 🛠️ `bin/` → Built DLLs and test executables

---

## 🔁 Static vs Shared Library

Handled via macros in the header:

```cpp
#if defined(AUDIO_SWITCHER_STATIC)
    // Static build: no export/import macros
#elif defined(AUDIO_SWITCHER_EXPORTS)
    // DLL build: export public symbols
#else
    // DLL usage: import public symbols
#endif
```

---

## 🧩 API Overview

### 🧾 `std::vector<AudioDevice> AudioManager::listOutputDevices();`

Returns all currently active playback (render) devices.

Each `AudioDevice` contains:

- `std::wstring id` — Unique system ID
- `std::wstring name` — Friendly display name
- `IMMDevice* device` — Raw COM pointer (optional use)

---

### 🎧 `bool AudioManager::setDefaultOutputDevice(const std::wstring& id);`

Sets the device with the given ID as the new default playback device.

| Parameter | Type           | Description                       |
|----------|----------------|-----------------------------------|
| `id`     | `std::wstring` | Device ID string from IMMDevice   |

Returns: `true` on success, `false` otherwise.

---

### 🔍 `std::wstring GetDeviceFriendlyName(IMMDevice* device);`

Retrieves the display name of an audio device.

---

### 🔎 `DeviceFormatInfo GetDeviceFormatInfo(IMMDevice* device);`

Returns format metadata:

- Sample rate (Hz)
- Bit depth (bits/sample)
- Channels
- Block alignment

---

### 📥 `IMMDevice* GetDefaultAudioPlaybackDevice();`

Gets a pointer to the system's current default output device.  
Don't forget to `SafeRelease()` it after use!

---

### 🔇 `bool SetDefaultPlaybackDeviceMute(bool mute);`

Mute or unmute the current default device.

| Parameter | Type  | Description     |
|----------|-------|-----------------|
| `mute`   | bool  | `true = mute`   |

---

### 🔈 `bool MuteDevice(IMMDevice* device, bool mute);`

Mute or unmute a specific device by its IMMDevice pointer.

| Parameter | Type          | Description       |
|-----------|---------------|-------------------|
| `device`  | `IMMDevice*`  | Device to control |
| `mute`    | `bool`        | `true = mute`     |

---

## 💡 Example Usage

```cpp
#include "AudioSwitcher/AudioSwitcher.h"
#include "Utility/DeviceUtils.h"
#include "Utility/COMInitializer.h"

int main() {
    Utility::COMInitializer comInit;

    auto devices = AudioSwitcher::AudioManager::listOutputDevices();
    for (const auto& d : devices)
        std::wcout << L"🎧 Device: " << d.name << L"\n";

    auto defaultDev = Utility::GetDefaultAudioPlaybackDevice();
    Utility::MuteDevice(defaultDev, true);  // Mute default device
    Utility::SafeRelease(defaultDev);
}
```

---

## 🔗 Integrating into Your Project

📖 See [LIBRARYUSAGE.md](./LIBRARYUSAGE.md) for full setup.

### Option 1: Subdirectory CMake

```cmake
add_subdirectory(WinAudioManager)
target_link_libraries(MyApp PRIVATE AudioSwitcherStatic)  # or AudioSwitcherShared
```

### Option 2: Copy Headers + Libs

- Copy `include/` + `lib/*.lib` + `bin/*.dll`
- Link statically or dynamically
- Define `AUDIO_SWITCHER_STATIC` if using static lib

---

## 📄 License

MIT License — Use freely in commercial, personal, and open-source projects.

---

## 🙏 Acknowledgments

Built on:

- 📚 Windows Core Audio APIs (IMMDevice, IAudioClient, etc.)
- 🙌 [tartakynov/audioswitch](https://github.com/tartakynov/audioswitch) for exposing `IPolicyConfig.h`

Special thanks to the C++/COM open-source community 💙

---
