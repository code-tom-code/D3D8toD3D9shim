#pragma once

#include "ObjectMap.h"
#include "d3d9include.h"
#include "d3d8include.h"

#include "IDirect3D8Hook.h"
#include "IDirect3DBaseTexture8Hook.h"
#include "IDirect3DCubeTexture8Hook.h"
#include "IDirect3DDevice8Hook.h"
#include "IDirect3DIndexBuffer8Hook.h"
#include "IDirect3DPixelShader8Hook.h"
#include "IDirect3DResource8Hook.h"
#include "IDirect3DStateBlock8Hook.h"
#include "IDirect3DSurface8Hook.h"
#include "IDirect3DSwapChain8Hook.h"
#include "IDirect3DTexture8Hook.h"
#include "IDirect3DVertexBuffer8Hook.h"
#include "IDirect3DVertexShader8Hook.h"
#include "IDirect3DVolume8Hook.h"
#include "IDirect3DVolumeTexture8Hook.h"

#include "GlobalToggles.h"

#include <map>

static CRITICAL_SECTION cs = {0};

struct ScopeLock
{
	ScopeLock(CRITICAL_SECTION* const _pcs) : pcs(_pcs)
	{
		EnterCriticalSection(_pcs);
	}

	~ScopeLock()
	{
		LeaveCriticalSection(pcs);
	}

private:
	CRITICAL_SECTION* const pcs;
};

static std::map<IDirect3D9*, IDirect3D8Hook*> Direct3Ds;

void InitObjectMap()
{
	InitializeCriticalSection(&cs);
}

void DestroyObjectMap()
{
	EnterCriticalSection(&cs);
	LeaveCriticalSection(&cs);
	DeleteCriticalSection(&cs);
}

static inline void* Alloc(const unsigned size)
{
#ifdef USE_PAGE_ALLOCATOR
	return VirtualAlloc(NULL, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
#else
	return malloc(size);
#endif
}

template <typename T>
static inline void Dealloc(T* const deadObj)
{
#ifdef USE_PAGE_ALLOCATOR 
#ifdef PROTECT_AFTER_FREE
	DWORD oldProtect = 0;
	VirtualProtect(deadObj, sizeof(T), PAGE_NOACCESS, &oldProtect);
#else
	VirtualFree(deadObj, 0, MEM_RELEASE);
#endif
	free(deadObj);
#else // #ifdef USE_PAGE_ALLOCATOR
#endif // #ifdef USE_PAGE_ALLOCATOR
}

IDirect3D8Hook* ObjLookupCreate(IDirect3D9* ptr)
{
	if (!ptr)
		return NULL;

	const ScopeLock sl(&cs);

	auto it = Direct3Ds.find(ptr);
	if (it != Direct3Ds.end() )
	{
		it->second->AddRef();
		return it->second;
	}

	void* NewHookMemory = Alloc(sizeof(IDirect3D8Hook) );
	IDirect3D8Hook* NewHook = new (NewHookMemory) IDirect3D8Hook(ptr);
	Direct3Ds.insert(std::make_pair(ptr, NewHook) );
	return NewHook;
}

IDirect3DDevice8Hook* ObjLookupCreate(IDirect3DDevice9* ptr, IDirect3D8Hook* parent)
{
	if (!ptr)
		return NULL;

	const ScopeLock sl(&parent->cs);

	auto it = parent->Devices.find(ptr);
	if (it != parent->Devices.end() )
	{
		it->second->AddRef();
		return it->second;
	}

	// Aligned_malloc and placement-new are required here to guarantee 16-byte alignment:
	void* NewHookMemory = Alloc(sizeof(IDirect3DDevice8Hook) );
	IDirect3DDevice8Hook* NewDevice = new (NewHookMemory) IDirect3DDevice8Hook(ptr, parent);
	parent->Devices.insert(std::make_pair(ptr, NewDevice) );
	return NewDevice;
}

IDirect3DBaseTexture8Hook* ObjLookupCreate(IDirect3DBaseTexture9* ptr, IDirect3DDevice8Hook* parent)
{
	if (!ptr)
		return NULL;

	const ScopeLock sl(&parent->cs);

	auto it = parent->BaseTextures.find(ptr);
	if (it != parent->BaseTextures.end() )
	{
		it->second->AddRef();
		return it->second;
	}

	void* NewHookMemory = Alloc(sizeof(IDirect3DBaseTexture8Hook) );
	IDirect3DBaseTexture8Hook* NewHook = new (NewHookMemory) IDirect3DBaseTexture8Hook(ptr, parent);
	parent->BaseTextures.insert(std::make_pair(ptr, NewHook) );
	return NewHook;
}

IDirect3DCubeTexture8Hook* ObjLookupCreate(IDirect3DCubeTexture9* ptr, IDirect3DDevice8Hook* parent)
{
	if (!ptr)
		return NULL;

	const ScopeLock sl(&parent->cs);

	auto it = parent->CubeTextures.find(ptr);
	if (it != parent->CubeTextures.end() )
	{
		it->second->AddRef();
		return it->second;
	}

	void* NewHookMemory = Alloc(sizeof(IDirect3DCubeTexture8Hook) );
	IDirect3DCubeTexture8Hook* NewHook = new (NewHookMemory) IDirect3DCubeTexture8Hook(ptr, parent);
	parent->CubeTextures.insert(std::make_pair(ptr, NewHook) );
	return NewHook;
}

IDirect3DIndexBuffer8Hook* ObjLookupCreate(IDirect3DIndexBuffer9* ptr, IDirect3DDevice8Hook* parent)
{
	if (!ptr)
		return NULL;

	const ScopeLock sl(&parent->cs);

	auto it = parent->IndexBuffers.find(ptr);
	if (it != parent->IndexBuffers.end() )
	{
		it->second->AddRef();
		return it->second;
	}

	void* NewHookMemory = Alloc(sizeof(IDirect3DIndexBuffer8Hook) );
	IDirect3DIndexBuffer8Hook* NewHook = new (NewHookMemory) IDirect3DIndexBuffer8Hook(ptr, parent);
	parent->IndexBuffers.insert(std::make_pair(ptr, NewHook) );
	return NewHook;
}

IDirect3DPixelShader8Hook* ObjLookupCreate(IDirect3DPixelShader9* ptr, IDirect3DDevice8Hook* parent)
{
	if (!ptr)
		return NULL;

	const ScopeLock sl(&parent->cs);

	auto it = parent->PixelShaders.find(ptr);
	if (it != parent->PixelShaders.end() )
	{
		it->second->AddRef();
		return it->second;
	}

	void* NewHookMemory = Alloc(sizeof(IDirect3DPixelShader8Hook) );
	IDirect3DPixelShader8Hook* NewHook = new (NewHookMemory) IDirect3DPixelShader8Hook(ptr, parent);
	parent->PixelShaders.insert(std::make_pair(ptr, NewHook) );
	return NewHook;
}

IDirect3DResource8Hook* ObjLookupCreate(IDirect3DResource9* ptr, IDirect3DDevice8Hook* parent)
{
	if (!ptr)
		return NULL;

	const ScopeLock sl(&parent->cs);

	auto it = parent->Resources.find(ptr);
	if (it != parent->Resources.end() )
	{
		it->second->AddRef();
		return it->second;
	}

	void* NewHookMemory = Alloc(sizeof(IDirect3DResource8Hook) );
	IDirect3DResource8Hook* NewHook = new (NewHookMemory) IDirect3DResource8Hook(ptr, parent);
	parent->Resources.insert(std::make_pair(ptr, NewHook) );
	return NewHook;
}

IDirect3DStateBlock8Hook* ObjLookupCreate(IDirect3DStateBlock9* ptr, IDirect3DDevice8Hook* parent)
{
	if (!ptr)
		return NULL;

	const ScopeLock sl(&parent->cs);

	auto it = parent->StateBlocks.find(ptr);
	if (it != parent->StateBlocks.end() )
	{
		it->second->AddRef();
		return it->second;
	}

	void* NewHookMemory = Alloc(sizeof(IDirect3DStateBlock8Hook) );
	IDirect3DStateBlock8Hook* NewHook = new (NewHookMemory) IDirect3DStateBlock8Hook(ptr, parent);
	parent->StateBlocks.insert(std::make_pair(ptr, NewHook) );
	return NewHook;
}

IDirect3DSurface8Hook* ObjLookupCreate(IDirect3DSurface9* ptr, IDirect3DDevice8Hook* parent)
{
	if (!ptr)
		return NULL;

	const ScopeLock sl(&parent->cs);

	auto it = parent->Surfaces.find(ptr);
	if (it != parent->Surfaces.end() )
	{
		it->second->AddRef();
		return it->second;
	}

	void* NewHookMemory = Alloc(sizeof(IDirect3DSurface8Hook) );
	IDirect3DSurface8Hook* NewHook = new (NewHookMemory) IDirect3DSurface8Hook(ptr, parent);
	parent->Surfaces.insert(std::make_pair(ptr, NewHook) );
	return NewHook;
}

IDirect3DSwapChain8Hook* ObjLookupCreate(IDirect3DSwapChain9* ptr, IDirect3DDevice8Hook* parent)
{
	if (!ptr)
		return NULL;

	const ScopeLock sl(&parent->cs);

	auto it = parent->SwapChains.find(ptr);
	if (it != parent->SwapChains.end() )
	{
		it->second->AddRef();
		return it->second;
	}

	void* NewHookMemory = Alloc(sizeof(IDirect3DSwapChain8Hook) );
	IDirect3DSwapChain8Hook* NewHook = new (NewHookMemory) IDirect3DSwapChain8Hook(ptr, parent);
	parent->SwapChains.insert(std::make_pair(ptr, NewHook) );
	return NewHook;
}

IDirect3DTexture8Hook* ObjLookupCreate(IDirect3DTexture9* ptr, IDirect3DDevice8Hook* parent)
{
	if (!ptr)
		return NULL;

	const ScopeLock sl(&parent->cs);

	auto it = parent->Textures.find(ptr);
	if (it != parent->Textures.end() )
	{
		it->second->AddRef();
		return it->second;
	}

	void* NewHookMemory = Alloc(sizeof(IDirect3DTexture8Hook) );
	IDirect3DTexture8Hook* NewHook = new (NewHookMemory) IDirect3DTexture8Hook(ptr, parent);
	parent->Textures.insert(std::make_pair(ptr, NewHook) );
	return NewHook;
}

IDirect3DVertexBuffer8Hook* ObjLookupCreate(IDirect3DVertexBuffer9* ptr, IDirect3DDevice8Hook* parent)
{
	if (!ptr)
		return NULL;

	const ScopeLock sl(&parent->cs);

	auto it = parent->VertexBuffers.find(ptr);
	if (it != parent->VertexBuffers.end() )
	{
		it->second->AddRef();
		return it->second;
	}

	void* NewHookMemory = Alloc(sizeof(IDirect3DVertexBuffer8Hook) );
	IDirect3DVertexBuffer8Hook* NewHook = new (NewHookMemory) IDirect3DVertexBuffer8Hook(ptr, parent);
	parent->VertexBuffers.insert(std::make_pair(ptr, NewHook) );
	return NewHook;
}

/*IDirect3DVertexShader8Hook* ObjLookupCreate(IDirect3DVertexShader9* ptr, IDirect3DDevice8Hook* parent)
{
	if (!ptr)
		return NULL;

	const ScopeLock sl(&parent->cs);

	auto it = parent->VertexShaders.find(ptr);
	if (it != parent->VertexShaders.end() )
	{
		it->second->AddRef();
		return it->second;
	}

	void* NewHookMemory = Alloc(sizeof(IDirect3DVertexShader8Hook) );
	IDirect3DVertexShader8Hook* NewHook = new (NewHookMemory) IDirect3DVertexShader8Hook(ptr, parent);
	parent->VertexShaders.insert(std::make_pair(ptr, NewHook) );
	return NewHook;
}*/

IDirect3DVolume8Hook* ObjLookupCreate(IDirect3DVolume9* ptr, IDirect3DDevice8Hook* parent)
{
	if (!ptr)
		return NULL;

	const ScopeLock sl(&parent->cs);

	auto it = parent->Volumes.find(ptr);
	if (it != parent->Volumes.end() )
	{
		it->second->AddRef();
		return it->second;
	}

	void* NewHookMemory = Alloc(sizeof(IDirect3DVolume8Hook) );
	IDirect3DVolume8Hook* NewHook = new (NewHookMemory) IDirect3DVolume8Hook(ptr, parent);
	parent->Volumes.insert(std::make_pair(ptr, NewHook) );
	return NewHook;
}

IDirect3DVolumeTexture8Hook* ObjLookupCreate(IDirect3DVolumeTexture9* ptr, IDirect3DDevice8Hook* parent)
{
	if (!ptr)
		return NULL;

	const ScopeLock sl(&parent->cs);

	auto it = parent->VolumeTextures.find(ptr);
	if (it != parent->VolumeTextures.end() )
	{
		it->second->AddRef();
		return it->second;
	}

	void* NewHookMemory = Alloc(sizeof(IDirect3DVolumeTexture8Hook) );
	IDirect3DVolumeTexture8Hook* NewHook = new (NewHookMemory) IDirect3DVolumeTexture8Hook(ptr, parent);
	parent->VolumeTextures.insert(std::make_pair(ptr, NewHook) );
	return NewHook;
}

void ObjDestroy(IDirect3D8Hook* ptr)
{
	const ScopeLock sl(&cs);
	Direct3Ds.erase(ptr->d3d9);
	Dealloc(ptr);
}

void ObjDestroy(IDirect3DDevice8Hook* ptr)
{
	IDirect3D8Hook* parent = ptr->GetParentD3D();
	const ScopeLock sl(&parent->cs);
	parent->Devices.erase(ptr->GetUnderlyingDevice() );
	Dealloc(ptr);
}

void ObjDestroy(IDirect3DBaseTexture8Hook* ptr)
{
	IDirect3DDevice8Hook* parent = ptr->GetParentDevice();
	const ScopeLock sl(&parent->cs);
	parent->BaseTextures.erase(ptr->GetUnderlyingBaseTexture() );
	Dealloc(ptr);
}

void ObjDestroy(IDirect3DCubeTexture8Hook* ptr)
{
	IDirect3DDevice8Hook* parent = ptr->GetParentDevice();
	const ScopeLock sl(&parent->cs);
	parent->CubeTextures.erase(ptr->GetUnderlyingCubeTexture() );
	Dealloc(ptr);
}

void ObjDestroy(IDirect3DIndexBuffer8Hook* ptr)
{
	IDirect3DDevice8Hook* parent = ptr->GetParentDevice();
	const ScopeLock sl(&parent->cs);
	parent->IndexBuffers.erase(ptr->GetUnderlyingIndexBuffer() );
	Dealloc(ptr);
}

void ObjDestroy(IDirect3DPixelShader8Hook* ptr)
{
	IDirect3DDevice8Hook* parent = ptr->GetParentDevice();
	const ScopeLock sl(&parent->cs);
	parent->PixelShaders.erase(ptr->GetUnderlyingPixelShader() );
	Dealloc(ptr);
}

void ObjDestroy(IDirect3DResource8Hook* ptr)
{
	IDirect3DDevice8Hook* parent = ptr->GetParentDevice();
	const ScopeLock sl(&parent->cs);
	parent->Resources.erase(ptr->GetUnderlyingResource() );
	Dealloc(ptr);
}

void ObjDestroy(IDirect3DStateBlock8Hook* ptr)
{
	IDirect3DDevice8Hook* parent = ptr->GetParentDevice();
	const ScopeLock sl(&parent->cs);
	parent->StateBlocks.erase(ptr->GetUnderlyingStateBlock() );
	Dealloc(ptr);
}

void ObjDestroy(IDirect3DSurface8Hook* ptr)
{
	IDirect3DDevice8Hook* parent = ptr->GetParentDevice();
	const ScopeLock sl(&parent->cs);
	parent->Surfaces.erase(ptr->GetUnderlyingSurface() );
	Dealloc(ptr);
}

void ObjDestroy(IDirect3DSwapChain8Hook* ptr)
{
	IDirect3DDevice8Hook* parent = ptr->GetParentDevice();
	const ScopeLock sl(&parent->cs);
	parent->SwapChains.erase(ptr->GetUnderlyingSwapChain() );
	Dealloc(ptr);
}

void ObjDestroy(IDirect3DTexture8Hook* ptr)
{
	IDirect3DDevice8Hook* parent = ptr->GetParentDevice();
	const ScopeLock sl(&parent->cs);
	parent->Textures.erase(ptr->GetUnderlyingTexture() );
	Dealloc(ptr);
}

void ObjDestroy(IDirect3DVertexBuffer8Hook* ptr)
{
	IDirect3DDevice8Hook* parent = ptr->GetParentDevice();
	const ScopeLock sl(&parent->cs);
	parent->VertexBuffers.erase(ptr->GetUnderlyingVertexBuffer() );
	Dealloc(ptr);
}

void ObjDestroy(IDirect3DVertexShader8Hook* ptr)
{
	IDirect3DDevice8Hook* parent = ptr->GetParentDevice();
	const ScopeLock sl(&parent->cs);
	parent->VertexShaders.erase(ptr->GetUnderlyingVertexShader() );
	Dealloc(ptr);
}

void ObjDestroy(IDirect3DVolume8Hook* ptr)
{
	IDirect3DDevice8Hook* parent = ptr->GetParentDevice();
	const ScopeLock sl(&parent->cs);
	parent->Volumes.erase(ptr->GetUnderlyingVolume() );
	Dealloc(ptr);
}

void ObjDestroy(IDirect3DVolumeTexture8Hook* ptr)
{
	IDirect3DDevice8Hook* parent = ptr->GetParentDevice();
	const ScopeLock sl(&parent->cs);
	parent->VolumeTextures.erase(ptr->GetUnderlyingVolumeTexture() );
	Dealloc(ptr);
}
