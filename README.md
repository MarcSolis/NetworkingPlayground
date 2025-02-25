# Instalation guide

## Windows

1. Install **CMake** and **Visual Studio**
2. Navigate to the root folder of the project you want to build
   > e.g., .../NetworkingPlayground/ObjectSerialization
3. Open the **cmd window** on that folder and execute the following command:
```
cmake -B Solution/ -S Generator/
```
  > Alternatively you can install CMake GUI and configure from the editor if you don't want to use cmd.
This will generate the .sln file inside the "Solution" folder that you can open with VisualStudio.


## Linux

Although you can run the project from the console with the **make** command, in this guide we will see how to configure the project with VSCode.

1. Install **VSCode** and the following VSCode extensions:
   - C/C++ (Microsoft)
   - C/C++ Extension Pack (Microsoft)
   - CMake Tools (Microsoft)
   - C/C++ Themes (Microsoft) *optional*
   - Cmake (twxs) *optional*
2. Open **CMake Tools** extension settings and change the "*Cmake: Build Directory*" to:
```
${workspaceFolder}/Solution
```
3. On VSCode, click  *File* -> *OpenFolder* and select the root folder of the project you want to build 
    > e.g., .../NetworkingPlayground/ObjectSerialization
4. A prompt will appear asking for the **CMakeLists.txt** as shown below. 
If the prompt does not appear, you can manually open the command line (Ctrl + Shift + P) and type "***CMake: Configure***". This will open the same prompt shown below.
  <img width="987" alt="LinuxPrompt2" src="https://github.com/user-attachments/assets/91865519-2d18-44fb-954d-5e62add03af7" />

Once the **CMakeLists.txt** is located, it will automatically build the MakeFile and create the .vscode files, and the project will be properly set up.
