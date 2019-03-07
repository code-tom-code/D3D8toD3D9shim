# D3D8 to D3D9 Shim

This proxy DLL acts as a shim to convert calls between the Direct3D8 API to the Direct3D9 API (this is a drop-in proxy replacement for Microsoft's official **d3d8.dll**). This project is intended to be a fully working API shim for all of Direct3D 8.0 and Direct3D 8.1. It is intended to work with real games and other programs, and eventually should support a high level of compatibility with a wide range of existing D3D8 games and programs. This shim uses the same hardware acceleration as the programs underlying it, and there is no software emulation of D3D8 features, instead all existing D3D8 features and shaders get converted to D3D9 features and shaders internally.

Why would you want to use this shim, when Windows already supports running Direct3D8 games and programs on pretty much all modern versions of Windows? I personally use this shim to make various games compatible with other things that require D3D9 support, such as Microsoft's PIX and Intel's GPA. This shim will also make your D3D8 games and programs work with any program that tries to hook **d3d9.dll** to inject overlays or to perform video capture or screenshotting functionality.

## Known Tested & Supported Products
- Advent Rising
- The Elder Scrolls III: Morrowind (Game of the Year edition)
- Unreal Tournament 2004

# For Users

## Getting Started (Users)

If you're just interested in using this proxy DLL to shim your D3D8 games and programs to use D3D9, then all you need is the [d3d8.dll](Release/d3d8.dll) binary. Just download it, and place it in the directory directly next to the EXE you want to proxy. You'll know that it's picked it up if when you launch your game or program, you should now see a message box pop up that says that the proxying was successful.

Note that although I've built this binary in Release x86 mode, with the CRT statically-linked, and using the v141_xp toolset with no SSE requirements, I still cannot guarantee that this DLL will work for you on your system.

# For Developers

## Getting Started (Developers)

Download the repo and open **d3d8tod3d9shim.sln** in Visual Studio 2017 or higher (or if you really want to you could hack the project and solution files to use an earlier version of Visual Studio, this codebase should be compatible all the way back to Visual Studio 2003).

### Prerequisites

You will need to have Visual Studio 2017 or higher installed, both for building this project, and for runtime JIT as well.
```
Get Visual Studio 2017 from Microsoft, it's free now for noncommercial use and it's still arguably the best IDE on Windows for C++ programming!
```

### Building

Open the **d3d8tod3d9shim.sln** solution in Visual Studio 2017 (or higher).
Once the project and all associated files have finished loading/initializing, select the desired Solution Configuration for your target program.
```
Currently supported targets are:
Debug - Win32
Release - Win32

Note that there are no x64 targets. This is because Direct3D8 was never supported for x64 (unlike DirectDraw7, which *does* have an x64 implementation), and the DirectX8.1 SDK did not have x64 libraries to link against, and there are no x64 D3D8 programs in the wild to my knowledge.
```

If you run into any compilation errors with the C/C++ code, they're likely due to the language conformance setting in Visual Studio. This project was written without using many modern C/C++ features (basically the C++11 featureset), but also not necessarily conforming to any future strictness that may be added into the C++ standard.

If you run into any linker errors, check and make sure that all of the **.c** and **.cpp** files are properly added to the project and that they are properly marked as "C/C++ compiler" under the "Item Type" property for each file so that they properly participate in the build.

### Settings

The settings for this proxy DLL are hardcoded in as preprocessor macros in [GlobalToggles.h](d3d8tod3d9shim/GlobalToggles.h). Feel free to edit that file by commenting out, or uncommenting various lines to change the functionality of the shim to your liking.

Know that if you enable the **ADDITIONAL_D3DX_DISASM** setting, then you will need to have the D3DX9 header files and libraries installed and your Visual Studio configured to have the Include Directories and Libraries Directories set up correctly. If you do not have them already, you can get them here in the DirectX SDK (June 2010): https://www.microsoft.com/en-us/download/details.aspx?id=6812

### Note

As a note, this project uses a much older (and probably less correct) version of the **ShaderAnalysis** library. A much more recent version of this same library can be found within the [Software_D3D9](https://github.com/code-tom-code/Software_D3D9) project's repository.

## Deployment

After building, go to the appropriate build output files folder depending on your build configuration:
```
Debug/Win32: .\Debug\
Release/Win32: .\Release\
```
Copy the newly built **d3d8.dll** file from that folder and paste it into the directory next to the program that you wish to test with it. Depending on how this program is launched and how it is configured to load DLLs, you may want to instead place the proxy DLL into the working directory of your test program rather than the immediate directory in which the executable resides. Due to how DLL loading works on Windows (unless programs specify otherwise), the proxy shim **d3d8.dll** should be picked up rather than the true Microsoft **d3d8.dll**.

## Built With

* [Wrappit] (https://www.codeproject.com/Articles/16541/Create-your-Proxy-DLLs-automatically) - Used to generate the proxy DLL template (which has since been very heavily modified).

## Authors

* **Tom Lopes** - *Initial work* - [code-tom-code](https://github.com/code-tom-code)

See also the list of [contributors](https://github.com/code-tom-code/D3D8toD3D9shim/contributors) who participated in this project.

## License

This project is licensed under the zLib/LibPNG License - see the [LICENSE.txt](LICENSE.txt) file for details

## Acknowledgments

* Thanks to Michael Chourdakis for his Wrappit code (https://www.codeproject.com/Articles/16541/Create-your-Proxy-DLLs-automatically). It's been useful for me for years now for easily generating a starting-point for a proxy DLL.
* Thanks to Microsoft and MSDN for having awesome documentation for the Direct3D8/Direct3D9/9Ex APIs and the HLSL shader bytecode format docs to go off of. They have really helped with navigating edge cases and solving tricky problems.
