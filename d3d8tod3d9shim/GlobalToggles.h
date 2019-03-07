#pragma once

// See here: https://msdn.microsoft.com/en-us/library/windows/desktop/bb204851(v=vs.85).aspx#EnumAdapterModes_Changes
#define DEFAULT_BACKBUFFER_FORMAT D3DFMT_X8R8G8B8

// Comment this out to un-force Windowed mode:
#ifdef _DEBUG
	#define FORCE_WINDOWED_MODE 1 // Windowed mode forcing is recommended for debugging if you only have one monitor!
#endif

// Comment this out to enable proper D3D8 and D3D9 object releasing behavior. Leave it uncommented to leak a ton of memory by eliding proper COM Release behavior.
// Disabling releases fixes a *lot* of crash-bugs that are caused by improper AddRef/Release behavior right now. Tracking all of these bugs down
// individually is a huge amount of time, but luckily for us most D3D8 games didn't use all of their 2GB 32-bit address space as it was, so it's not
// a huge deal for most games to leak D3D8/D3D9 objects. This is mostly a big deal for longer play sessions, or for open-world games (like TES3: Morrowind) where
// lots of textures and models are constantly being created and (not) destroyed.
#define DISABLE_RELEASES 1

// If this is not commented out, then the full-page allocator is being used (uses more memory than the heap allocator)
// #define USE_PAGE_ALLOCATOR 1

// Comment this out to save memory and performance when deleting objects:
#ifdef USE_PAGE_ALLOCATOR
	#define PROTECT_AFTER_FREE 1
#endif

// Comment out to disable shader disassembling in the ShaderAnalysis library (this is useful for debugging)
#define DISASM_SHADER 1

// Comment out this to disable D3DX disassembly (probably will be more correct and more easily readable than the custom formatted disassembly from the ShaderAnalysis library, better for debugging, but requires the D3DX9 library).
// This feature is only for debugging and is not needed for proper operation whatsoever!
#ifdef DISASM_SHADER
	// #define ADDITIONAL_D3DX_DISASM 1
#endif

// How long to wait on queries to return before bailing out with an error:
#define QUERY_TIMEOUT_TIME_MILLISECONDS 100

// This is useful for knowing whether the proxy DLL is being picked up or not
//#ifdef _DEBUG
	#define MESSAGEBOX_ON_CREATEDEVICE 1
//#endif
