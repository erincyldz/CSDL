# C Gravitation Simulation with using SDL2 Library.

## **Installing dependencies and building the project for the first time**.
This project created using WSL Ubuntu LTS on a windows platform. But before you clone and use make to build the project, you have to make some configuration if this is the first time using C/GCC/SDL in your machine.



### **Dependencies**
Before you get started, make sure you run ```sudo apt-get update && sudo apt-get upgrade``` to update the main packages in your system.
- **Build Essentials**: Install build essentials in your linux subsystem by simply running ```sudo apt-get install build-essential```.
- **SDL2 Dev. Lib**: Install SDL2 Dev packages in your machine by simply running ```sudo apt-cache search libsdl2 && sudo apt-get install libsdl2-dev```.



### **Building the project**.
After completing the previous steps, now you can build and run the project by running the code ```make``` in the root folder of the project. The make configurations are __basically__ configured in the Makefile, but it might require further steps for optimization and generalization of the build steps. I don't know how to write a Makefile bro.

### **Running the Project**.
After ```make``` command, a folder named __build__ will be created under your root directory with the __main__ executable file in it. Just run ```./build/main``` to run the project.

###Setting up the formatter
You have to install LLVM for formatting your code. Refer to [this](https://llvm.org/docs/GettingStarted.html).

### **Known Issues and Fixes**
(PROBABLY ONLY FOR WSL USERS)When SDL library tries to popup a textbox or etc. via using xServer over windows, it fails and gets a core dump fail. To fix this, you need to install the correct GUI package for your wsl. It is most probably gets fixed with  ```sudo apt install zenity ``` but it may depend on the linux version and distribution you are currently using. For further questions, go and search it on Google, Stackoverflow or ask ChatGPT. Don't bother me. 