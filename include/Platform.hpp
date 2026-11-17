#pragma once

namespace game::platform
{
// Lets the console show the logger's ANSI colors. Needed on Windows, where the classic console
// prints the escape codes literally until virtual terminal processing is switched on.
// Does nothing on other platforms.
void enableConsoleColors();
}  // namespace game::platform
