# C++ Gravitation Simulation with using SDL2 Library.

## **Installing dependencies and building the project for the first time**.
This project created using WSL Ubuntu 22.04 LTS on a windows platform. But before you clone and use make to build the project, you have to make some configuration if this is the first time using C/GCC/SDL in your machine.


### **Dependencies**
Before you get started, make sure you run ```sudo apt-get update && sudo apt-get upgrade``` to update the main packages in your system.
- **Build Essentials**: Install build essentials in your linux subsystem by simply running ```sudo apt-get install build-essential```.
- **SDL2 Dev. Lib**: Install SDL2 Dev packages in your machine by simply running ```sudo apt-cache search libsdl2 && sudo apt-get install libsdl2-dev```.
- **SDL2 Mixer Related Dependencies**: In order to build the project, you need SDL2 Mixer library installed in your system. For this, first create a folder to install SDL2_Mixer library, go to [here](https://github.com/libsdl-org/SDL_mixer/releases) and install **version 2.8.0 tar.gz**. Extract the compressed file to your folder. For example lets say our folder name is dependencies, after the extraction, yor folders should look like this ```dependencies/SDL2_mixer-2.8.0```. Before you continue, you have to install a couple of libraries to make 'cmake' work. Simply run  ``` sudo apt install libopusfile-dev && sudo apt install libxmp-dev && sudo apt install fluidsynth libfluidsynth-dev && sudo apt install wavpack libwavpack-dev ```. After the installations are completed, ```mkdir build && cd build && cmake .. && make && sudo make install ``` to install the libraries to your system. Then run ```sudo apt install libsdl2-mixer-2.0-0```. Then you can build and run CSDL project!
- **SDL2 TTF Related Dependencies**: In order to use font library of SDL (SDL2_ttf), you have to install it just like the SDL2 Mixer. First, ```sudo apt-get install libfreetype6-dev``` after ```sudo apt-get update&& sudo apt-get upgrade```. Then, go to [here](https://github.com/libsdl-org/SDL_ttf/releases) and install version **version 2.24.0 tar.gz** to your machine, then simple build the project with cmake and make and ```sudo make install``` to install the library to your system. Then you can build CSDL project without any errors, i guess.
### **Building the project**.
After completing the previous steps, now you can build and run the project by running the code ```mkdir build && cd build && cmake .. && make``` in the root folder of the project. The make configurations are __basically__ configured in the CMake file, but it might require further steps for optimization and generalization of the build steps. I don't know how to write a CMake bro.

### **Running the Project**.
The executable is created as `bin/engine` and the `assets` folder is copied next to it, so it can be started from any directory. The first CMake configure downloads Dear ImGui (pinned version + checksum); for offline builds pass `-DCSDL_IMGUI_DIR=/path/to/imgui`.

```
./bin/engine                          # default scene, sandbox panel on the right
./bin/engine --random-objects=40      # 40 random circles/rectangles
./bin/engine --scene my_test.txt      # start from a saved scene (R reloads it)
./bin/engine --no-gravity --time-scale=0.25
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
You have to install LLVM for formatting your code. Refer to [this](https://llvm.org/docs/GettingStarted.html).




### **Known Issues and Fixes**
**THIS PROJECT IS CREATED USING UBUNTU 22.04LTS. SOMEHOW, IN OTHER UBUNTU RELEASES, I COULD NOT GET MY TOOLCHAIN SETTED UP TO BUILD THE PROJECT BECAUSE I AM NOT CAPABLE OF FIXING THOSE KIND OF PROBLEMS(FOR NOW).**

- (**PROBABLY ONLY FOR WSL USERS**)When SDL library tries to popup a textbox or etc. via using xServer over windows, it fails and gets a core dump fail. To fix this, you need to install the correct GUI package for your wsl. It is most probably gets fixed with  ```sudo apt install zenity ``` but it may depend on the linux version and distribution you are currently using. For further questions, go and search it on Google, Stackoverflow or ask ChatGPT. Don't bother me. 
- In systems other than Ubuntu 22.04LTS, some building problems might occur (for example, i could not build Logger class because i had to include each C++ standard library by hand, such as **`#include <array>`** **`#include <unordered_map>`**). If you know how to fix it, please feel free to contribute to this file.
