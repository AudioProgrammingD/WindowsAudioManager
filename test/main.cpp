#include "AudioSwitcher/AudioSwitcher.h"
#include "Utility/DeviceUtils.h"
#include "Utility/COMInitializer.h"
#include "Utility/SafeRelease.h"

#include <iostream>
#include <limits>

using namespace AudioSwitcher;
using namespace Utility;

int main()
{
    COMInitializer comInit;

    try
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
        return 0;
    }
    catch (const std::exception &ex)
    {
        std::cerr << "[Fatal Error] " << ex.what() << "\n";
        return 1;
    }
}
