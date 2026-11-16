#include "Platform.hpp"

// Kept in its own file: <windows.h> defines macros such as ERROR, min and max that clash with
// names used elsewhere in the code.
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#endif

namespace game::platform
{
void enableConsoleColors()
{
#ifdef _WIN32
    const DWORD streams[] = {STD_OUTPUT_HANDLE, STD_ERROR_HANDLE};
    for (DWORD id : streams)
    {
        HANDLE handle = GetStdHandle(id);
        DWORD mode = 0;
        if (handle != INVALID_HANDLE_VALUE && GetConsoleMode(handle, &mode))
            SetConsoleMode(handle, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    }
#endif
}
}  // namespace game::platform
