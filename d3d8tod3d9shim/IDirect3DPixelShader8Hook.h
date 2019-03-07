#pragma once

#include "IDirect3DDevice8Hook.h"
#include <vector>

struct IDirect3DPixelShader8Hook
{
public:
	IDirect3DPixelShader8Hook(LPDIRECT3DPIXELSHADER9 _realObject, IDirect3DDevice8Hook* _parentDevice) : realObject(_realObject), parentDevice(_parentDevice), refCount(1)
	{
	}

	inline LPDIRECT3DPIXELSHADER9 GetUnderlyingPixelShader(void) const
	{
		return realObject;
	}

	virtual ~IDirect3DPixelShader8Hook()
	{
		ObjDestroy(this);
	}

	/*** IUnknown methods ***/
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE QueryInterface(THIS_ REFIID riid, void** ppvObj);
    virtual COM_DECLSPEC_NOTHROW ULONG STDMETHODCALLTYPE AddRef(THIS);
    virtual COM_DECLSPEC_NOTHROW ULONG STDMETHODCALLTYPE Release(THIS);

    /*** IDirect3DPixelShader9 methods ***/
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetDevice(THIS_ IDirect3DDevice8** ppDevice);
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetFunction(THIS_ void* pData,UINT* pSizeOfData);

	inline IDirect3DDevice8Hook* GetParentDevice(void) const
	{
		return parentDevice;
	}

	void SetOriginalShaderBytecode(const DWORD* pFunction);

protected:
	LPDIRECT3DPIXELSHADER9 realObject;
	IDirect3DDevice8Hook* parentDevice;
	unsigned __int64 refCount;

	std::vector<DWORD> originalShaderBytecode;
};
