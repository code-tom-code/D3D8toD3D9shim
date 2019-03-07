#pragma once

#include "IDirect3DDevice8Hook.h"

struct IDirect3DStateBlock8Hook
{
public:
	IDirect3DStateBlock8Hook(LPDIRECT3DSTATEBLOCK9 _realObject, IDirect3DDevice8Hook* _parentDevice) : realObject(_realObject), parentDevice(_parentDevice), refCount(1)
	{
	}

	~IDirect3DStateBlock8Hook()
	{
		ObjDestroy(this);
	}

	/*** IUnknown methods ***/
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE QueryInterface(THIS_ REFIID riid, void** ppvObj);
    virtual COM_DECLSPEC_NOTHROW ULONG STDMETHODCALLTYPE AddRef(THIS);
    virtual COM_DECLSPEC_NOTHROW ULONG STDMETHODCALLTYPE Release(THIS);

    /*** IDirect3DStateBlock9 methods ***/
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetDevice(THIS_ IDirect3DDevice8** ppDevice);
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE Capture(THIS);
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE Apply(THIS);

	inline IDirect3DDevice8Hook* GetParentDevice(void) const
	{
		return parentDevice;
	}

	inline LPDIRECT3DSTATEBLOCK9 GetUnderlyingStateBlock(void) const
	{
		return realObject;
	}

protected:
	LPDIRECT3DSTATEBLOCK9 realObject;
	IDirect3DDevice8Hook* parentDevice;
	unsigned __int64 refCount;

public:
	// Extra device state from D3D8 that is not present in D3D9:
	D3DSTATEBLOCKTYPE StateBlockType;
	UINT BaseVertexIndex; // Set in d3d8 via SetIndices (used in D3D9 for Draw* calls)

	// Render states from D3D8 that aren't in D3D9:
	DWORD D3DRS_LINEPATTERN;
	DWORD D3DRS_ZVISIBLE;
	DWORD D3DRS_EDGEANTIALIAS;
	DWORD D3DRS_ZBIAS;
	DWORD D3DRS_SOFTWAREVERTEXPROCESSING;
	DWORD D3DRS_PATCHSEGMENTS;
};
