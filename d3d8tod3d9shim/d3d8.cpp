
#undef UNICODE
#undef _UNICODE
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "d3d9include.h"
#include "d3d8include.h"
#pragma pack(1)

#include "IDirect3D8Hook.h"
#include "ObjectMap.h"

// Forward-declaring GetSystemDirectoryA here rather than including <sysinfoapi.h> because the include seems to fail with the Windows-XP-compatible version of the compiler that we're using
WINBASEAPI _Success_(return != 0 && return < uSize) UINT WINAPI GetSystemDirectoryA(_Out_writes_to_opt_(uSize, return + 1) LPSTR lpBuffer, _In_ UINT uSize);

HINSTANCE hLThis = 0;
HINSTANCE hL = 0;
FARPROC p[5] = {0};

BOOL WINAPI DllMain(_In_ HINSTANCE hInst, _In_ DWORD reason, _In_ LPVOID /*lpvReserved*/)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		hLThis = hInst;

		// There doesn't seem to be a D3D8 for x64, so we only need to ever worry about having an x86 version of this DLL to load

		{
			char realDLLPath[MAX_PATH];
			if (!GetSystemDirectoryA(realDLLPath, ARRAYSIZE(realDLLPath) ) )
			{
				return FALSE;
			}
#pragma warning(push)
#pragma warning(disable:4996) // warning C4996: 'strcat': This function or variable may be unsafe. Consider using strcat_s instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.
			strcat(realDLLPath, "\\d3d8.dll");
#pragma warning(pop)
			hL = LoadLibraryA(realDLLPath);
		}
		if (!hL) 
			return FALSE;


		p[0] = GetProcAddress(hL,"DebugSetMute");
		p[1] = GetProcAddress(hL,"Direct3D8EnableMaximizedWindowedModeShim");
		p[2] = GetProcAddress(hL,"Direct3DCreate8");
		p[3] = GetProcAddress(hL,"ValidatePixelShader");
		p[4] = GetProcAddress(hL,"ValidateVertexShader");

		InitObjectMap();
	}

	if (reason == DLL_PROCESS_DETACH)
	{
		FreeLibrary(hL);
		DestroyObjectMap();
	}

	return TRUE;
}

// DebugSetMute
extern "C" __declspec(naked) HRESULT WINAPI __E__0__()
{
__asm
	{
	jmp p[0*4];
	}
}

// Direct3D8EnableMaximizedWindowedModeShim
extern "C" __declspec(naked) void __stdcall __E__1__()
{
__asm
	{
	jmp p[1*4];
	}
}

// Direct3DCreate8
extern "C" IDirect3D8* WINAPI __E__2__(UINT SDKVersion)
{
#ifdef _DEBUG
	MessageBoxA(NULL, "D3DCreate8", NULL, NULL);
#endif

	LPDIRECT3D9 d3d9 = Direct3DCreate9(D3D9_SDK_VERSION);

	if (!d3d9)
	{
		return NULL;
	}

	IDirect3D8Hook* newHook = ObjLookupCreate(d3d9);
	return newHook;
}

// ValidatePixelShader
extern "C" __declspec(naked) void __stdcall __E__3__()
{
__asm
	{
	jmp p[3*4];
	}
}

// ValidateVertexShader
extern "C" __declspec(naked) void __stdcall __E__4__()
{
__asm
	{
	jmp p[4*4];
	}
}

