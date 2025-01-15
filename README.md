# C++ Gravitation Simulation with using SDL2 Library.

## **Installing dependencies and building the project for the first time**.
This project created using WSL Ubuntu LTS on a windows platform. But before you clone and use make to build the project, you have to make some configuration if this is the first time using C/GCC/SDL in your machine.


### **Dependencies**
Before you get started, make sure you run ```sudo apt-get update && sudo apt-get upgrade``` to update the main packages in your system.
- **Build Essentials**: Install build essentials in your linux subsystem by simply running ```sudo apt-get install build-essential```.
- **SDL2 Dev. Lib**: Install SDL2 Dev packages in your machine by simply running ```sudo apt-cache search libsdl2 && sudo apt-get install libsdl2-dev```.
- **SDL2 Mixer Related Dependencies**: In order to build the project, you need SDL2 Mixer library installed in your system. For this, first create a folder to install SDL2_Mixer library, go to [here](https://github.com/libsdl-org/SDL_mixer/releases) and install **version 2.8.0 tar.gz**. Extract the compressed file to your folder. For example lets say our folder name is dependencies, after the extraction, yor folders should look like this ```dependencies/SDL2_mixer-2.8.0```. Before you continue, you have to install a couple of libraries to make 'cmake' work. Simply run  ``` sudo apt install libopusfile-dev && sudo apt install libxmp-dev && sudo apt install fluidsynth libfluidsynth-dev && sudo apt install wavpack libwavpack-dev ```. After the installations are completed, ```mkdir build && cd build && cmake .. && make && sudo make install ``` to install the libraries to your system. Then you can build and run CSDL project!
- **SDL2 TTF Related Dependencies**: In order to use font library of SDL (SDL2_ttf), you have to install it just like the SDL2 Mixer. First, ```sudo apt-get install libfreetype6-dev``` after ```sudo apt-get update&& sudo apt-get upgrade```. Then, go to [here](https://github.com/libsdl-org/SDL_ttf/releases) and install version **version 2.24.0 tar.gz** to your machine, then simple build the project with cmake and make and ```sudo make install``` to install the library to your system. Then you can build CSDL project without any errors, i guess.
### **Building the project**.
After completing the previous steps, now you can build and run the project by running the code ```mkdir build && cd build && cmake .. && make``` in the root folder of the project. The make configurations are __basically__ configured in the CMake file, but it might require further steps for optimization and generalization of the build steps. I don't know how to write a CMake bro.

### **Running the Project**.
After ```make``` command, an executable with the name of the project will be created under ```build``` folder. Just run ```./build/project_name``` on your terminal.

### **Setting up the formatter**
You have to install LLVM for formatting your code. Refer to [this](https://llvm.org/docs/GettingStarted.html).




### **Known Issues and Fixes**
(PROBABLY ONLY FOR WSL USERS)When SDL library tries to popup a textbox or etc. via using xServer over windows, it fails and gets a core dump fail. To fix this, you need to install the correct GUI package for your wsl. It is most probably gets fixed with  ```sudo apt install zenity ``` but it may depend on the linux version and distribution you are currently using. For further questions, go and search it on Google, Stackoverflow or ask ChatGPT. Don't bother me. 
