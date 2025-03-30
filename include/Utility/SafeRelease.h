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

#include <windows.h>

namespace Utility
{
    /**
     * @brief Releases a COM object safely and sets the pointer to nullptr.
     */
    template <typename T>
    void SafeRelease(T *&ptr)
    {
        if (ptr)
        {
            ptr->Release();
            ptr = nullptr;
        }
    }
}
