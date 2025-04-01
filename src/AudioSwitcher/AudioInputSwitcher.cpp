#include "AudioSwitcher/AudioInputSwitcher.h"
#include "AudioSwitcher/IPolicyConfig.h"

#include <mmdeviceapi.h>
#include <functiondiscoverykeys_devpkey.h>
#include <iostream>
#include <comdef.h>

namespace AudioSwitcher
{
    /**
     * @brief Lists all active audio input (capture) devices like microphones.
     *
     * @return std::vector<AudioInputDevice> A list of devices with their IDs and friendly names.
     * @throws std::runtime_error If any COM operation fails.
     */
    std::vector<AudioInputDevice> AudioInputManager::listInputDevices()
    {
        std::vector<AudioInputDevice> devices;

        IMMDeviceEnumerator *pEnum = nullptr;
        IMMDeviceCollection *pDevices = nullptr;

        try
        {
            HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
                                          __uuidof(IMMDeviceEnumerator), (void **)&pEnum);
            if (FAILED(hr))
                throw std::runtime_error("[x] Failed to create device enumerator.");

            // Input devices → eCapture
            hr = pEnum->EnumAudioEndpoints(eCapture, DEVICE_STATE_ACTIVE, &pDevices);
            if (FAILED(hr))
                throw std::runtime_error("[x] Failed to enumerate input devices.");

            UINT count = 0;
            hr = pDevices->GetCount(&count);
            if (FAILED(hr))
                throw std::runtime_error("[x] Failed to get input device count.");
            if (count == 0)
                throw std::runtime_error("[x] No input devices found.");

            for (UINT i = 0; i < count; ++i)
            {
                IMMDevice *pDevice = nullptr;
                LPWSTR deviceId = nullptr;

                hr = pDevices->Item(i, &pDevice);
                if (FAILED(hr))
                    continue;

                hr = pDevice->GetId(&deviceId);
                if (FAILED(hr) && pDevice)
                {
                    pDevice->Release();
                    continue;
                }

                IPropertyStore *pStore = nullptr;
                hr = pDevice->OpenPropertyStore(STGM_READ, &pStore);
                if (FAILED(hr) && pDevice)
                {
                    pDevice->Release();
                    continue;
                }

                PROPVARIANT prop;
                PropVariantInit(&prop);
                hr = pStore->GetValue(PKEY_Device_FriendlyName, &prop);
                if (SUCCEEDED(hr))
                {
                    AudioInputDevice device;
                    device.id = deviceId;
                    device.name = prop.pwszVal;
                    device.device = pDevice;

                    devices.push_back(std::move(device));
                }
                else
                {
                    if (pDevice)
                        pDevice->Release();
                }

                PropVariantClear(&prop);
                pStore->Release();
            }

            pDevices->Release();
            pEnum->Release();

            return devices;
        }
        catch (const std::exception &e)
        {
            if (pDevices)
                pDevices->Release();
            if (pEnum)
                pEnum->Release();

            throw std::runtime_error(e.what());
        }
    }

    /**
     * @brief Sets the given device ID as the system default input (recording) device.
     *
     * @param deviceId The input device ID from GetId().
     * @return true on success, false otherwise.
     */
    bool AudioInputManager::setDefaultInputDevice(const std::wstring &deviceId)
    {
        IPolicyConfig *pPolicyConfig = nullptr;

        try
        {
            HRESULT hr = CoCreateInstance(__uuidof(CPolicyConfigClient), nullptr, CLSCTX_ALL,
                                          __uuidof(IPolicyConfig), (void **)&pPolicyConfig);
            if (FAILED(hr) || !pPolicyConfig)
                throw std::runtime_error("[x] Failed to create IPolicyConfig COM object.");

            HRESULT hr1 = pPolicyConfig->SetDefaultEndpoint(deviceId.c_str(), eConsole);
            HRESULT hr2 = pPolicyConfig->SetDefaultEndpoint(deviceId.c_str(), eMultimedia);
            HRESULT hr3 = pPolicyConfig->SetDefaultEndpoint(deviceId.c_str(), eCommunications);

            pPolicyConfig->Release();

            return SUCCEEDED(hr1) && SUCCEEDED(hr2) && SUCCEEDED(hr3);
        }
        catch (const std::exception &e)
        {
            if (pPolicyConfig)
                pPolicyConfig->Release();

            std::wcerr << L"[x] Error: " << e.what() << L"\n";
            return false;
        }
    }

} // namespace AudioSwitcher
