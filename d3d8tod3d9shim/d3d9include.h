#pragma once

#ifdef _DEBUG
	#define D3D_DEBUG_INFO 1
#else
	#undef D3D_DEBUG_INFO
#endif

// warning C4530: C++ exception handler used, but unwind semantics are not enabled. Specify /EHsc
#pragma warning(disable:4530)

#undef UNICODE
#undef _UNICODE
#define WIN32_LEAN_AND_MEAN

#define INITGUID 1
	#include <d3d9.h>
#undef INITGUID

#pragma comment(lib, "d3d9.lib")

#ifdef ADDITIONAL_D3DX_DISASM
#include <d3dx9.h>
#ifdef _DEBUG
	#pragma comment(lib, "d3dx9d.lib")
#else
	#pragma comment(lib, "d3dx9.lib")
#endif
#endif // #ifdef ADDITIONAL_D3DX_DISASM

// This is hard-coded from the June 2010 version of the DXSDK (the last version to fully support D3D9 that was a standalone from the Windows SDK)
#define D3D9_SDK_VERSION 32

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

#ifdef _DEBUG
	#define DbgBreakPrint(x) MessageBoxA(NULL, __FILE__ ":" TOSTRING(__LINE__) "\n" x, "Error", NULL); OutputDebugStringA(__FILE__ ":" TOSTRING(__LINE__) "\n" x "\n"); __debugbreak()
#else
	#define DbgBreakPrint(x) OutputDebugStringA(x "\n")
#endif
