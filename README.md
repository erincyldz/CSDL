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

From a *Developer PowerShell for VS*:
```
git clone https://github.com/erincyldz/CSDL.git
cd CSDL
cmake --preset windows
cmake --build --preset windows
.\bin\Release\engine.exe
```
Alternatively, open the folder in Visual Studio, select the `windows` preset and build. The DLLs and the `assets` folder are copied next to `engine.exe`.

Keep the clone in a short path such as `C:\dev\CSDL`. Some dependencies fail to build when their paths go over the Windows 260-character limit.

### **Continuous integration**
`.github/workflows/ci.yml` runs on every push and pull request to `main`: gcc (Release) and clang (Debug + ASan/UBSan) builds, a short headless smoke test of the real app (`SDL_VIDEODRIVER=dummy`), the benchmarks and a clang-format check. To run the workflow locally before pushing, use [act](https://github.com/nektos/act) (needs Docker): `act -j build`.

### **Running the Project**.
The executable is created as `bin/engine` and the `assets` folder is copied next to it, so it can be started from any directory. The first CMake configure downloads Dear ImGui (pinned version + checksum); for offline builds pass `-DCSDL_IMGUI_DIR=/path/to/imgui`.

```
./bin/engine                          # default scene, sandbox panel on the right
./bin/engine --random-objects=40      # 40 random circles/rectangles
./bin/engine --scene my_test.txt      # start from a saved scene (R reloads it)
./bin/engine --no-gravity --time-scale=0.25
./bin/engine --skip-menu              # go straight to the simulation
./bin/engine --config csdl.example.conf
./bin/engine --help                   # all options with their defaults
```

Every option can be given on the command line (`--key=value`, `--key value`, `--flag`, `--no-flag`) or in a config file with one `key = value` per line (see `csdl.example.conf`). The log level is set with `--log-level debug|info|warning|error` (or the old `-d`, `-i`, `-w`, `-e` shortcuts).

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
- `Config`: runtime configuration (CLI + config file). `Game`: state machine and main loop.
- `bench/`: standalone micro benchmarks (no SDL) comparing the old and new trail buffer, collision lookup and circle drawing. Build with `cmake -DCSDL_BUILD_BENCHMARKS=ON ..`, run `./bin/csdl_bench`.

### **Setting up the formatter**
Install it with `sudo apt-get install clang-format` (CI checks with clang-format 18) and format with
`clang-format -i include/*.hpp src/*.cpp bench/*.cpp` before committing.

### **Known Issues and Fixes**

- (**PROBABLY ONLY FOR WSL USERS**)When SDL library tries to popup a textbox or etc. via using xServer over windows, it fails and gets a core dump fail. To fix this, you need to install the correct GUI package for your wsl. It is most probably gets fixed with  ```sudo apt install zenity ``` but it may depend on the linux version and distribution you are currently using. For further questions, go and search it on Google, Stackoverflow or ask ChatGPT. Don't bother me. 
