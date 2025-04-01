#pragma once

// Export/import macro — same rules apply as your output device switcher.
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
#include <mmdeviceapi.h> // IMMDevice*

namespace AudioSwitcher
{
    /**
     * @brief Struct representing an audio input (recording) device.
     */
    struct AudioInputDevice
    {
        std::wstring id;             ///< The unique ID of the input device.
        std::wstring name;           ///< Friendly name (e.g., "Microphone", "Line In").
        IMMDevice *device = nullptr; ///< Raw pointer to the device.

        AudioInputDevice() = default;

        ~AudioInputDevice()
        {
            if (device)
                device->Release();
        }

        // Prevent copy to avoid double-release
        AudioInputDevice(const AudioInputDevice &) = delete;
        AudioInputDevice &operator=(const AudioInputDevice &) = delete;

        // Enable move semantics
        AudioInputDevice(AudioInputDevice &&other) noexcept
        {
            id = std::move(other.id);
            name = std::move(other.name);
            device = other.device;
            other.device = nullptr;
        }

        AudioInputDevice &operator=(AudioInputDevice &&other) noexcept
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
     * @brief Class for managing audio input (recording) devices.
     */
    class AUDIO_SWITCHER_API AudioInputManager
    {
    public:
        /**
         * @brief Lists all active audio input (capture) devices.
         *
         * @return std::vector<AudioInputDevice> List of available microphones.
         */
        static std::vector<AudioInputDevice> listInputDevices();

        /**
         * @brief Sets the given device as the system's default input device.
         *
         * @param deviceId ID of the input device to set as default.
         * @return true if successful, false otherwise.
         */
        static bool setDefaultInputDevice(const std::wstring &deviceId);
    };

} // namespace AudioSwitcher
