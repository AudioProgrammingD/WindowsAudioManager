#include <iostream>
#include "AudioSwitcher/AudioSwitcher.h"

/**
 * @brief A dummy function that forces an exported symbol and provides a simple output.
 *
 * Marked with AUDIO_SWITCHER_API to ensure it is exported when building the DLL.
 */
extern "C" AUDIO_SWITCHER_API void ForceExportSymbol()
{
    std::cout << "[AudioSwitcher] ForceExportSymbol() called (dummy export)." << std::endl;
}
