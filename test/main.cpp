#include "AudioSwitcher/AudioSwitcher.h"
#include <AudioSwitcher/AudioInputSwitcher.h>
#include "Utility/DeviceUtils.h"
#include "Utility/COMInitializer.h"
#include "Utility/SafeRelease.h"

#include <iostream>
#include <limits>

using namespace AudioSwitcher;
using namespace Utility;

void functionOutput()
{
    std::vector<AudioDevice> devices = AudioManager::listOutputDevices();

    std::cout << "Available Playback Devices:\n\n";
    for (size_t i = 0; i < devices.size(); ++i)
    {
        // Convert std::wstring to std::string (simple lossy narrow)
        std::wstring ws = devices[i].name;
        std::wstring name = devices[i].name;
        std::wstring id = devices[i].id;
        std::wcout << L"ID:" << id << L"\n"
                   << "Name:" << name << "\n\n";
        name = GetDeviceFriendlyName(devices[i].device);
        auto fmt = GetDeviceFormatInfo(devices[i].device);
        if (fmt.valid)
        {
            std::wcout << L"  Sample Rate : " << fmt.sampleRate << L" Hz\n";
            std::wcout << L"  Channels    : " << fmt.channels << L"\n";
            std::wcout << L"  Bit Depth   : " << fmt.bitDepth << L"\n";
            std::wcout << L"  Block Align : " << fmt.blockAlign << L"\n";
        }
    }

    std::cout << "\nWhat would you like to do?\n";
    std::cout << "  [1] Mute Default Device\n";
    std::cout << "  [2] Mute Specific Device\n";
    std::cout << "  [3] Switch Default Device\n";
    std::cout << "Enter choice: ";

    int action = 0;
    std::cin >> action;

    if (action == 1)
    {
        bool success = SetDefaultPlaybackDeviceMute(true);
        std::cout << (success ? "[+] Default device muted.\n" : "[x] Failed to mute default device.\n");
    }
    else if (action == 2)
    {
        std::cout << "Enter the number of the device to mute: ";
        int index;
        std::cin >> index;

        if (index >= 0 && index < devices.size())
        {
            bool success = MuteDevice(devices[index].device, true);
            std::wcout << (success ? L"[+] Muted: " : L"[x] Failed to mute: ")
                       << GetDeviceFriendlyName(devices[index].device) << L"\n";
        }
        else
        {
            std::cout << "[x] Invalid index.\n";
        }
    }
    else if (action == 3)
    {
        std::cout << "Enter the number of the device to set as default: ";
        int index;
        std::cin >> index;

        if (index >= 0 && index < devices.size())
        {
            bool success = AudioManager::setDefaultOutputDevice(devices[index].id);
            std::wcout << (success ? L"[+] Default set to: " : L"[x] Failed to switch to: ")
                       << GetDeviceFriendlyName(devices[index].device) << L"\n";
        }
        else
        {
            std::cout << "[x] Invalid index.\n";
        }
    }
    else
    {
        std::cout << "[x] Invalid choice.\n";
    }

    devices.clear();
}

void functionInput()
{
    std::vector<AudioDevice> devices = AudioManager::listOutputDevices();

    std::cout << "[+] Available Playback Devices:\n\n";
    for (size_t i = 0; i < devices.size(); ++i)
    {
        std::wcout << L"[" << i << L"] " << devices[i].name << L" [" << devices[i].id << L"]\n";
        auto fmt = GetDeviceFormatInfo(devices[i].device);
        if (fmt.valid)
        {
            std::wcout << L"    Sample Rate : " << fmt.sampleRate << L" Hz\n";
            std::wcout << L"    Channels    : " << fmt.channels << L"\n";
            std::wcout << L"    Bit Depth   : " << fmt.bitDepth << L"\n";
        }
    }

    std::cout << "\n[+] Available Input Devices:\n\n";
    std::vector<AudioInputDevice> inputDevices = AudioInputManager::listInputDevices();
    for (size_t i = 0; i < inputDevices.size(); ++i)
    {
        std::wcout << L"[" << i << L"] " << inputDevices[i].name << L" [" << inputDevices[i].id << L"]\n";
        auto fmt = GetDeviceFormatInfo(inputDevices[i].device);
        if (fmt.valid)
        {
            std::wcout << L"    Sample Rate : " << fmt.sampleRate << L" Hz\n";
            std::wcout << L"    Channels    : " << fmt.channels << L"\n";
            std::wcout << L"    Bit Depth   : " << fmt.bitDepth << L"\n";
        }
    }

    std::cout << "\n[+] What would you like to do?\n";
    std::cout << "  [1] Mute Default Playback Device\n";
    std::cout << "  [2] Mute Specific Playback Device\n";
    std::cout << "  [3] Switch Default Playback Device\n";
    std::cout << "  [4] Switch Default Input Device\n";
    std::cout << "  [5] Mute Default Input Device\n";
    std::cout << "  [6] Mute Specific Input Device\n";
    std::cout << "  [7] Unmute Default Playback Device\n";
    std::cout << "  [8] Unmute Specific Playback Device\n";
    std::cout << "  [9] Unmute Default Input Device\n";
    std::cout << " [10] Unmute Specific Input Device\n";
    std::cout << "Enter choice: ";

    int action = 0;
    std::cin >> action;

    switch (action)
    {
    case 1:
        std::cout << (SetDefaultPlaybackDeviceMute(true)
                          ? "[+] Default playback device muted.\n"
                          : "[x] Failed to mute default playback device.\n");
        break;

    case 2:
    {
        std::cout << "Enter playback device index to mute: ";
        int index;
        std::cin >> index;

        if (index >= 0 && index < devices.size())
        {
            bool success = MuteDevice(devices[index].device, true);
            std::wcout << (success ? L"[+] Muted: " : L"[x] Failed to mute: ") << GetDeviceFriendlyName(devices[index].device) << L"\n";
        }
        else
            std::cout << "[x] Invalid playback device index.\n";
        break;
    }

    case 3:
    {
        std::cout << "Enter playback device index to set as default: ";
        int index;
        std::cin >> index;

        if (index >= 0 && index < devices.size())
        {
            bool success = AudioManager::setDefaultOutputDevice(devices[index].id);
            std::wcout << (success ? L"[+] Default playback set to: " : L"[x] Failed to switch to: ")
                       << GetDeviceFriendlyName(devices[index].device) << L"\n";
        }
        else
            std::cout << "[x] Invalid playback device index.\n";
        break;
    }

    case 4:
    {
        std::cout << "Enter input device index to set as default: ";
        int index;
        std::cin >> index;

        if (index >= 0 && index < inputDevices.size())
        {
            bool success = AudioInputManager::setDefaultInputDevice(inputDevices[index].id);
            std::wcout << (success ? L"[+] Default input set to: " : L"[x] Failed to switch to: ") << inputDevices[index].name << L"\n";
        }
        else
            std::cout << "[x] Invalid input device index.\n";
        break;
    }

    case 5:
        std::cout << (SetDefaultInputDeviceMute(true)
                          ? "[+] Default input device muted.\n"
                          : "[x] Failed to mute default input device.\n");
        break;

    case 6:
    {
        std::cout << "Enter input device index to mute: ";
        int index;
        std::cin >> index;

        if (index >= 0 && index < inputDevices.size())
        {
            bool success = MuteDevice(inputDevices[index].device, true);
            std::wcout << (success ? L"[+] Muted: " : L"[x] Failed to mute: ") << GetDeviceFriendlyName(inputDevices[index].device) << L"\n";
        }
        else
            std::cout << "[x] Invalid input device index.\n";
        break;
    }

    case 7:
        std::cout << (SetDefaultPlaybackDeviceMute(false)
                          ? "[+] Default playback device unmuted.\n"
                          : "[x] Failed to unmute default playback device.\n");
        break;

    case 8:
    {
        std::cout << "Enter playback device index to unmute: ";
        int index;
        std::cin >> index;

        if (index >= 0 && index < devices.size())
        {
            bool success = MuteDevice(devices[index].device, false);
            std::wcout << (success ? L"[+] Unmuted: " : L"[x] Failed to unmute: ") << GetDeviceFriendlyName(devices[index].device) << L"\n";
        }
        else
            std::cout << "[x] Invalid playback device index.\n";
        break;
    }

    case 9:
        std::cout << (SetDefaultInputDeviceMute(false)
                          ? "[+] Default input device unmuted.\n"
                          : "[x] Failed to unmute default input device.\n");
        break;

    case 10:
    {
        std::cout << "Enter input device index to unmute: ";
        int index;
        std::cin >> index;

        if (index >= 0 && index < inputDevices.size())
        {
            bool success = MuteDevice(inputDevices[index].device, false);
            std::wcout << (success ? L"[+] Unmuted: " : L"[x] Failed to unmute: ") << GetDeviceFriendlyName(inputDevices[index].device) << L"\n";
        }
        else
            std::cout << "[x] Invalid input device index.\n";
        break;
    }

    default:
        std::cout << "[x] Invalid choice.\n";
        break;
    }
}

int main()
{
    COMInitializer comInit;

    try
    {
        // functionOutput();
        functionInput();
        return 0;
    }
    catch (const std::exception &ex)
    {
        std::cerr << "[Fatal Error] " << ex.what() << "\n";
        return 1;
    }
}
