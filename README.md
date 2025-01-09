# C++ Gravitation Simulation with using SDL2 Library.

## **Installing dependencies and building the project for the first time**.
This project created using WSL Ubuntu LTS on a windows platform. But before you clone and use make to build the project, you have to make some configuration if this is the first time using C/GCC/SDL in your machine.


### **Dependencies**
Before you get started, make sure you run ```sudo apt-get update && sudo apt-get upgrade``` to update the main packages in your system.
- **Build Essentials**: Install build essentials in your linux subsystem by simply running ```sudo apt-get install build-essential```.
- **SDL2 Dev. Lib**: Install SDL2 Dev packages in your machine by simply running ```sudo apt-cache search libsdl2 && sudo apt-get install libsdl2-dev```.
- **SDL2 Mixer Related Dependencies**: In order to build the project, you need SDL2 Mixer library installed in your system. For this, first create a folder to install SDL2_Mixer library, go to [here](https://github.com/libsdl-org/SDL_mixer/releases) and install version 2.8.0 tar.gz. Extract the compressed file to your folder. For example lets say our folder name is dependencies, after the extraction, yor folders should look like this ```dependencies/SDL2_mixer-2.8.0```. Before you continue, you have to install a couple of libraries to make 'cmake' work. Simply run  ``` sudo apt install libopusfile-dev && sudo apt install libxmp-dev && sudo apt install fluidsynth libfluidsynth-dev && sudo apt install wavpack libwavpack-dev ```. After the installations are completed, ```mkdir build && cd build && cmake .. && make && sudo make install ``` to install the libraries to your system. Then you can build and run CSDL project!

### **Building the project**.
After completing the previous steps, now you can build and run the project by running the code ```mkdir build && cd build && cmake .. && make``` in the root folder of the project. The make configurations are __basically__ configured in the CMake file, but it might require further steps for optimization and generalization of the build steps. I don't know how to write a CMake bro.

### **Running the Project**.
After ```make``` command, an executable with the name of the project will be created under ```build``` folder. Just run ```./build/project_name``` on your terminal.

### **Setting up the formatter**
You have to install LLVM for formatting your code. Refer to [this](https://llvm.org/docs/GettingStarted.html).

## **Commit Template**
Commit messages should be self explanatory with enough detail for developers, maintainers, testers and every other member of the development team to understand what exactly has changed with each commit.
- Commit type that will give first hints about commit change.
    - "[+]": New feature
    - "[#]": Bug fix
    - "[~]": Refactoring, general changes or others that doesn't fit first two 
- Summary is the must have and it has two parts in it. First part is the name of the module and the second part is the summary of the change.
- Description is made out of several parts, it can be considered as a template of explaining, giving enough detail on the changes about:
    - how algorithm works
    - what are the new dependencies
    - how this change solves the problem/creates solution
    - additional notes
    - relevant links (issues, other PRs etc.)
    - co-authors.

- Use the imperative mood in the summary line while writing commit messages. A properly written summary line for a commit message should be able to complete the following sentence: *"This commit will ..."*.

    - This commit will *change the search algorithm*.
    - This commit will ~~*the search algorithm is changed*~~.

Other than summary, all the parts are optional yet, it is highly encouraged to use them. Template and example can be found below:

**Template:**
```markdown
[~#+] Module Name Abbreviation(if needed): Summarize the change in less than 50 characters

What is changed (if needed):
- Explain new algorithm.
- Explain new dependencies.

Because (if needed):
- Explain the reasons you made this change
- Make a new bullet for each reason
- Each line should be under 72 characters
Remarks (if needed):
Include any additional notes, relevant links, or co-authors.(if needed)

Issue: #GITHUB_ISSUE_ID
```

**Example:**

```markdown
[~] search: Refactor algorithm

What is changed:
- The new algorithm's worst case O(n log(n)).
- "HyperSupperDupper Sort" library is not used anymore.

Because:
- Old algorithm was not optimized.
- System tend to crash with big data.

Remarks:
Corner cases were not covered. (see # 11231)

These changes should resolve issue # 1337.

This commit removed the getFirst(), so please stop using it!

Issue: #2019
```
Check [this](https://cbea.ms/git-commit/) article to read more about writing good commit messages.
Another cool git commit can be found [here](https://github.com/bitcoin/bitcoin/commit/eb0b56b19017ab5c16c745e6da39c53126924ed6).


### **Known Issues and Fixes**
(PROBABLY ONLY FOR WSL USERS)When SDL library tries to popup a textbox or etc. via using xServer over windows, it fails and gets a core dump fail. To fix this, you need to install the correct GUI package for your wsl. It is most probably gets fixed with  ```sudo apt install zenity ``` but it may depend on the linux version and distribution you are currently using. For further questions, go and search it on Google, Stackoverflow or ask ChatGPT. Don't bother me. 
