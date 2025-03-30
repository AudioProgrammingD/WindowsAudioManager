#pragma once

// Define export/import macro. When building the DLL, define AUDIO_SWITCHER_EXPORTS.
// On Windows, we want to export/import symbols only for the shared library.
// When building the static library, we don’t need any decoration.
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

#include <string>
#include <vector>
#include <mmdeviceapi.h> // Required for IMMDevice*

namespace AudioSwitcher
{
    /**
     * @brief Struct representing an audio output device.
     */
    struct AudioDevice
    {
        std::wstring id;             ///< The unique ID of the audio device (used by the system).
        std::wstring name;           ///< Friendly name shown to the user (e.g., "Speakers", "Headset").
        IMMDevice *device = nullptr; ///< Pointer to the actual device object (optional for advanced use).
        // Explicit default constructor to fix the issue
        AudioDevice() = default;
        // Optional: Destructor to safely release if ever needed
        ~AudioDevice()
        {
            if (device)
                device->Release();
        }

        // Optional: Copy constructor/operator deleted to avoid double-release
        AudioDevice(const AudioDevice &) = delete;
        AudioDevice &operator=(const AudioDevice &) = delete;

        // Allow move semantics (optional and useful)
        AudioDevice(AudioDevice &&other) noexcept
        {
            id = std::move(other.id);
            name = std::move(other.name);
            device = other.device;
            other.device = nullptr;
        }

        AudioDevice &operator=(AudioDevice &&other) noexcept
        {
            if (this != &other)
            {
                if (device)
                    device->Release();
                id = std::move(other.id);
                name = std::move(other.name);
                device = other.device;
                other.device = nullptr;
            }
            return *this;
        }
    };

    /**
     * @brief Main class responsible for managing audio output devices.
     */
    class AUDIO_SWITCHER_API AudioManager
    {
    public:
        /**
         * @brief Lists all active audio output devices on the system.
         *
         * @return std::vector<AudioDevice> List of devices with ID, name, and raw pointer.
         */
        static std::vector<AudioDevice> listOutputDevices();

        /**
         * @brief Sets the given device as the default playback device.
         *
         * @param deviceId The device ID string.
         * @return true if successful, false otherwise.
         */
        static bool setDefaultOutputDevice(const std::wstring &deviceId);
    };

} // namespace AudioSwitcher
