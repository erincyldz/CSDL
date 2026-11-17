# C++ Gravitation Simulation with using SDL2 Library.

[![CI](https://github.com/erincyldz/CSDL/actions/workflows/ci.yml/badge.svg)](https://github.com/erincyldz/CSDL/actions/workflows/ci.yml)

## **Installing dependencies and building the project**

### **Dependencies**
On Ubuntu 24.04 (native or WSL) everything comes from apt:
```
sudo apt-get update
sudo apt-get install build-essential cmake libsdl2-dev libsdl2-mixer-dev libsdl2-ttf-dev
```
On older releases (e.g. Ubuntu 22.04) the SDL2_mixer / SDL2_ttf packages are too old and ship no CMake config files. Build [SDL2_mixer 2.8](https://github.com/libsdl-org/SDL_mixer/releases) and [SDL2_ttf 2.24](https://github.com/libsdl-org/SDL_ttf/releases) from source instead (`mkdir build && cd build && cmake .. && make && sudo make install`); `/usr/local` is already on the CMake search path.

### **Building the project**
```
cmake -S . -B build        # Release by default
cmake --build build -j
```
- Debug build: `-DCMAKE_BUILD_TYPE=Debug`.
- Compiler warnings are shown but don't stop the build. CI builds with `-DCSDL_WERROR=ON` (warnings are errors), so turn it on locally before opening a PR.
- Libraries installed somewhere unusual: `-DCMAKE_PREFIX_PATH=/path/to/prefix`.
- The same with presets: `cmake --preset release && cmake --build --preset release`. There are also `debug` (warnings as errors, output in `build-debug/bin`) and `asan` (AddressSanitizer + UBSan, output in `build-asan/bin`) presets.

### **Windows (native)**
Needs Visual Studio 2022 or newer with the *Desktop development with C++* workload, which already contains CMake and vcpkg. SDL2, SDL2_mixer and SDL2_ttf are installed by vcpkg from `vcpkg.json`, with the versions pinned. The first configure builds them, which takes a few minutes; later configures reuse vcpkg's cache.

**Visual Studio, VS Code or CLion:** open the folder and pick the `windows` (or `windows-debug`) preset. The IDE sets up the compiler itself.

**Command line:** from any terminal (PowerShell, cmd, Windows Terminal), no IDE needed:
```
git clone https://github.com/erincyldz/CSDL.git
cd CSDL
scripts\build-windows.cmd
.\bin\engine.exe
```
The script finds Visual Studio (or Build Tools), loads its 64-bit compiler environment and runs the CMake preset. Use `-Config Debug` for a debug build (output in `build-windows-debug\bin`) and `-Run` to start the app after building. To call CMake yourself, open *x64 Native Tools Command Prompt for VS* from the Start menu and run `cmake --preset windows` and `cmake --build --preset windows`.
The presets use Ninja, so the same setup works with any Visual Studio version (2022, 2026, Build Tools). The IDE and the command line can share one build folder. The executable, the DLLs and the `assets` folder end up in `bin`, as on Linux. CMake stops with a clear message when it is run from a 32-bit prompt or without `VCPKG_ROOT`.

Keep the clone in a short path such as `C:\dev\CSDL`. Some dependencies fail to build when their paths go over the Windows 260-character limit.

### **Continuous integration**
`.github/workflows/ci.yml` runs on every push and pull request to `main`: gcc (Release) and clang (Debug + ASan/UBSan) builds, a short headless smoke test of the real app (`SDL_VIDEODRIVER=dummy`), the benchmarks and a clang-format check. To run the workflow locally before pushing, use [act](https://github.com/nektos/act) (needs Docker): `act -j build`.

### **Running the Project**.
The executable is created as `bin/engine` and the `assets` folder is copied next to it, so it can be started from any directory. The first CMake configure downloads Dear ImGui (pinned version + checksum); for offline builds pass `-DCSDL_IMGUI_DIR=/path/to/imgui`.

```
./bin/engine                          # default scene, sandbox panel on the right
./bin/engine --random-objects=40      # 40 random circles/rectangles
./bin/engine --scene my_test.txt      # start from a saved scene (R reloads it)
./bin/engine --preset orbits          # start from a built-in preset (see --help for the list)
./bin/engine --random-objects=40 --seed=7   # the same random scene on every start
./bin/engine --no-gravity --time-scale=0.25
./bin/engine --skip-menu              # go straight to the simulation
./bin/engine --config csdl.example.conf
./bin/engine --help                   # all options with their defaults
```

Every option can be given on the command line (`--key=value`, `--key value`, `--flag`, `--no-flag`) or in a config file with one `key = value` per line (see `csdl.example.conf`). The log level is set with `--log-level debug|info|warning|error` (or the old `-d`, `-i`, `-w`, `-e` shortcuts).

### **Headless mode**
`--headless` runs only the physics: no window, no sound, no rendering, and SDL is never initialized, so it works over SSH, in containers and in CI without a display. It runs a fixed number of physics steps as fast as possible, prints statistics and exits.

```
./bin/engine --headless                                    # default scene, 10 s simulated
./bin/engine --headless --preset container --no-gravity --duration 60
./bin/engine --headless --random-objects 100 --seed 7 --report-interval 0.5
./bin/engine --headless --scene my_test.txt --save-scene after.txt
```

- `--duration` is simulated time in seconds (default 10). With `--physics-hz` it sets the number of steps.
- `--report-interval` prints a statistics line every N simulated seconds (default 1, `0` = only the final line). Each line shows the object and collision counts and the kinetic, potential and total energy, with the drift from the start.
- `--save-scene` writes the final state to a scene file. The windowed app can load it with `--scene` to look at it.
- The world is `--window-width` x `--window-height` (no sandbox panel takes space from it).
- With the same `--seed`, scene and build, two runs print the same numbers; only the `wall time` line differs. Without `--seed`, the seed that was used is printed so a run can be repeated.
- Exit codes: `0` success, `1` invalid options or a scene file that can't be loaded or saved (unlike the windowed app, there is no fallback to the default scene), `2` the simulation diverged (a position, velocity or the total energy became NaN or infinite).

### **Sandbox**
The panel on the right (toggle with `F1`) is meant for testing the physics:

- **Mouse tool**: *Select / drag* - click to select, drag to move, release to throw (or just place it, if "Throw on release" is off). Stable objects can be dragged too. *Spawn* - click to place an object from the spawn template, drag to give it an initial velocity. Right click always deletes the object under the cursor.
- **Selected object**: edit radius / size, position, velocity, mass, restitution, stable flag and color live; stop, duplicate or delete it.
- **Spawn template**: shape, size, mass (or mass = area), restitution, stable, color, aim factor.
- **Objects**: list of all objects; click a row to select it.
- **Scene**: built-in presets (default, empty, random, circular orbits, head-on collisions, Newton's cradle, container) and saving / loading scene files.
- **Physics / Rendering & audio**: every runtime option, applied immediately.
- **Stats**: kinetic, potential and total energy, total momentum and an energy graph. A flat energy line is the quickest sanity check for a physics change.
- Pause, single step (`N`) and a time scale slider (slow motion / fast forward without changing the physics step size).

Scene files are plain text, one object per line, `#` starts a comment:
```
circle name=Sun    x=400 y=300 radius=25 mass=1000 restitution=0.5 stable=1 color=255,200,0
circle name=Planet x=550 y=300 vx=0 vy=-80 radius=6 mass=1
rect   name=Wall   x=100 y=300 width=20 height=400 stable=1
```
Missing keys use defaults (mass defaults to the area).

### **Controls**
| Key / action | Effect |
|---|---|
| Mouse (menu) | Play, Settings, Exit |
| Left mouse (in game) | Select / drag / throw, or spawn (see Sandbox) |
| Right mouse | Delete the object under the cursor |
| `P` | Pause / resume |
| `N` | Single physics step while paused |
| `R` | Reload the current scene or preset |
| `Q` | Add a random object (up to `--max-objects`) |
| `SPACE` | Kick every movable object in a random direction |
| `DEL` / `BACKSPACE` | Delete the selected object |
| `F1` | Show / hide the sandbox panel |
| `ESC` | Back to the menu (quits from the main menu) |

### **Code layout**
- `PhysicsWorld`, `Collision`, `GameObject`, `Shape`, `Vector2D`: SDL-free physics core (impulse-based collisions with positional correction, softened N-body gravity, energy/momentum statistics).
- `Sandbox` (mouse tools), `SceneIO` (scene files), `Scenario` (presets): SDL-free, testable editor logic.
- `Window`, `SdlRenderer` (`IRenderer`), `Input`, `Audio`, `FrameClock`, `Menu`: platform layer, SDL resources are owned through RAII wrappers in `SdlRaii.hpp`.
- `ImGuiLayer`, `SandboxPanel`: Dear ImGui integration and the sandbox window.
- `Config`: runtime configuration (CLI + config file). `Game`: state machine and main loop. `Headless`: the `--headless` runner.
- CMake builds the SDL-free parts (physics, `Sandbox`, `SceneIO`, `Scenario`, `Config`, `Logger`, `Headless`) as the `csdl_core` library, and the `engine` executable adds the SDL and ImGui parts on top. Sources are listed explicitly in `CMakeLists.txt`, so a new `.cpp` file has to be added to one of the two lists.
- `bench/`: standalone micro benchmarks (no SDL) comparing the old and new trail buffer, collision lookup and circle drawing. Build with `cmake -DCSDL_BUILD_BENCHMARKS=ON ..`, run `./bin/csdl_bench`.

### **Setting up the formatter**
Install it with `sudo apt-get install clang-format` (CI checks with clang-format 18) and format with
`clang-format -i include/*.hpp src/*.cpp bench/*.cpp` before committing.

### **Known Issues and Fixes**

- (**PROBABLY ONLY FOR WSL USERS**)When SDL library tries to popup a textbox or etc. via using xServer over windows, it fails and gets a core dump fail. To fix this, you need to install the correct GUI package for your wsl. It is most probably gets fixed with  ```sudo apt install zenity ``` but it may depend on the linux version and distribution you are currently using. For further questions, go and search it on Google, Stackoverflow or ask ChatGPT. Don't bother me. 
