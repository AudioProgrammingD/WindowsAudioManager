#pragma once
// Define export/import macro for the AudioSwitcher library.
// When building the DLL, define AUDIO_SWITCHER_EXPORTS so that this class is exported.
// Otherwise, it will be imported.
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
#include <mmdeviceapi.h>
#include "Utility/DeviceFormatInfo.h"

namespace Utility
{
    /**
     * @brief Retrieves the friendly name of an audio device.
     *
     * This function uses the device's property store to extract the user-visible name,
     * such as "Speakers (Realtek Audio)" or "Headphones".
     *
     * @param device Pointer to a valid IMMDevice.
     * @return std::wstring Friendly name of the device, or "Unknown" if retrieval fails.
     */
    AUDIO_SWITCHER_API std::wstring GetDeviceFriendlyName(IMMDevice *device);

    /**
     * @brief Retrieves audio format information (bit depth, sample rate, channels, etc.) for a device.
     *
     * Internally uses IAudioClient::GetMixFormat to get the shared-mode default audio format.
     *
     * @param device Pointer to a valid IMMDevice.
     * @return DeviceFormatInfo Struct containing audio format fields.
     *         Check `valid` field to confirm if data was successfully retrieved.
     */
    AUDIO_SWITCHER_API DeviceFormatInfo GetDeviceFormatInfo(IMMDevice *device);

    /**
     * @brief Retrieves the system's current default audio playback (render) device.
     *
     * This corresponds to the "default device" shown in Windows sound settings (used for system audio).
     * Caller is responsible for releasing the returned pointer using `SafeRelease()`.
     *
     * @return IMMDevice* Pointer to the default device. Returns nullptr on failure.
     */
    AUDIO_SWITCHER_API IMMDevice *GetDefaultAudioPlaybackDevice();

    /**
     * @brief Retrieves the system's current default audio **input** (capture) device.
     *
     * This corresponds to the default microphone shown in Windows sound settings.
     * Caller is responsible for releasing the returned pointer using `SafeRelease()`.
     *
     * @return IMMDevice* Pointer to the default input device. Returns nullptr on failure.
     */
    AUDIO_SWITCHER_API IMMDevice *GetDefaultAudioInputDevice();

    /**
     * @brief Mutes or unmutes the default audio playback device.
     *
     * This uses IAudioEndpointVolume COM interface to control the system volume mute state.
     *
     * @param mute True to mute, false to unmute.
     * @return true if successful, false otherwise.
     */
    AUDIO_SWITCHER_API bool SetDefaultPlaybackDeviceMute(bool mute);

    /**
     * @brief Mutes or unmutes the default audio input (microphone) device.
     *
     * @param mute True to mute, false to unmute.
     * @return True if successful, false otherwise.
     */
    AUDIO_SWITCHER_API bool SetDefaultInputDeviceMute(bool mute);

    /**
     * @brief Mutes or unmutes the given audio playback device.
     *
     * This uses the IAudioEndpointVolume COM interface to mute the specified device.
     *
     * @param device Pointer to the IMMDevice to mute/unmute.
     * @param mute True to mute, false to unmute.
     * @return true if successful, false otherwise.
     */
    AUDIO_SWITCHER_API bool MuteDevice(IMMDevice *device, bool mute);
}
