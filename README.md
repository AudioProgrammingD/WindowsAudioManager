# 🎧 WinAudioManager

**WinAudioManager** is a lightweight C++17 library for controlling audio output devices on **Windows**.

It wraps the Windows Core Audio APIs (COM-based) to provide a simple, modern, and safe interface for:

- 🔎 Enumerating playback and recording devices
- ✅ Switching default playback or input device
- 🔇 Muting/unmuting specific or default devices
- 📊 Fetching audio format metadata (bit depth, sample rate, channels)

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
- 🔇 Mute/unmute default or specific device (input/output)
- 🛠️ RAII-style `COMInitializer` helper
- 🔄 Build as either static `.lib` or shared `.dll + .lib`

---

## 📁 Project Structure

```bash
WinAudioManager/
├── include/
│   ├── AudioSwitcher/AudioSwitcher.h           # Playback (output)
│   ├── AudioSwitcher/AudioInputSwitcher.h      # Input (microphones)
│   └── Utility/
│       ├── COMInitializer.h
│       ├── DeviceUtils.h
│       ├── DeviceFormatInfo.h
│       └── SafeRelease.h
├── src/
│   ├── AudioSwitcher/AudioSwitcher.cpp
│   ├── AudioSwitcher/AudioInputSwitcher.cpp
│   └── Utility/
│       ├── COMInitializer.cpp
│       └── DeviceUtils.cpp
├── test/
│   └── main.cpp
├── bin/         # Built DLLs and test apps
├── lib/         # Static/shared libraries
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

### 🎙️ `bool AudioInputManager::setDefaultInputDevice(const std::wstring& id);`

Sets the device with the given ID as the new default **input (recording)** device.

| Parameter | Type           | Description                            |
|-----------|----------------|----------------------------------------|
| `id`      | `std::wstring` | Device ID string (from IMMDevice::GetId) |

**Returns:** `true` if successful, `false` otherwise.

---

### 🎙️ `std::vector<AudioDevice> AudioInputManager::listInputDevices();`

Lists all **active audio input (capture)** devices (e.g., microphones).

**Returns:**  
`std::vector<AudioDevice>` — each containing:

- `id`: unique device identifier
- `name`: friendly display name
- `device`: raw `IMMDevice*` pointer (can be used for mute/format)

---

### 🎙️ `IMMDevice* GetDefaultAudioInputDevice();`

Retrieves the system’s **default input** (microphone) device.

- ⚠️ You must `SafeRelease()` the returned pointer after use.
- ✅ COM-safe and handles errors internally.

**Returns:** `IMMDevice*` (nullptr on failure)

---

### 🔇 `bool SetDefaultInputDeviceMute(bool mute);`

Mute or unmute the **default input device**.

| Parameter | Type | Description         |
|-----------|------|---------------------|
| `mute`    | bool | `true = mute`, `false = unmute` |

**Returns:** `true` if successful, `false` otherwise.

---

## 📘 API Reference

### 🔊 Output (Playback) Devices

| Function | Description |
|----------|-------------|
| `AudioManager::listOutputDevices()` | Lists active playback devices |
| `AudioManager::setDefaultOutputDevice(id)` | Sets default output device |
| `GetDefaultAudioPlaybackDevice()` | Returns current default output device |
| `SetDefaultPlaybackDeviceMute(bool)` | Mute/unmute default output |
| `MuteDevice(device, mute)` | Mute/unmute any output device |
| `GetDeviceFriendlyName(device)` | Retrieves friendly name |
| `GetDeviceFormatInfo(device)` | Returns format metadata |

---

### 🎙️ Input (Microphone) Devices

| Function | Description |
|----------|-------------|
| `AudioInputManager::listInputDevices()` | Lists active input devices |
| `AudioInputManager::setDefaultInputDevice(id)` | Sets default input device |
| `GetDefaultAudioInputDevice()` | Gets current default input |
| `SetDefaultInputDeviceMute(bool)` | Mute/unmute default input |
| `MuteDevice(device, mute)` | Mute/unmute specific input device (same method) |

---

## 🔄 Input vs Output API Comparison

This library provides a **symmetric API** for working with both **Playback (Output)** and **Input (Recording)** devices, making it intuitive and consistent.

| 🧩 Function Type                          | 🎧 Playback (Output)                             | 🎙️ Input (Recording)                               |
|------------------------------------------|--------------------------------------------------|----------------------------------------------------|
| 🔍 List Devices                          | `AudioManager::listOutputDevices()`              | `AudioInputManager::listInputDevices()`            |
| ✅ Set Default Device                    | `AudioManager::setDefaultOutputDevice(id)`       | `AudioInputManager::setDefaultInputDevice(id)`     |
| 🎯 Get Default Device Pointer           | `GetDefaultAudioPlaybackDevice()`                | `GetDefaultAudioInputDevice()`                     |
| 📛 Get Friendly Name                     | `GetDeviceFriendlyName(IMMDevice* device)`       | ✅ Same for both                                    |
| 📊 Get Audio Format Info                 | `GetDeviceFormatInfo(IMMDevice* device)`         | ✅ Same for both                                    |
| 🔇 Mute Default Device                   | `SetDefaultPlaybackDeviceMute(bool mute)`        | `SetDefaultInputDeviceMute(bool mute)`             |
| 🔇 Mute Specific Device (by pointer)     | `MuteDevice(IMMDevice* device, bool mute)`       | ✅ Same for both                                    |

> 💡 All APIs are RAII-friendly and designed with safe COM lifecycle management in mind.
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
