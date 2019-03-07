#pragma once

#include "IDirect3DDevice8Hook.h"

struct IDirect3DVertexShader8Hook
{
public:
	IDirect3DVertexShader8Hook(LPDIRECT3DVERTEXSHADER9 _realObject, LPDIRECT3DVERTEXDECLARATION9 _realDecl, IDirect3DDevice8Hook* _parentDevice) : realObject(_realObject), realDecl(_realDecl), parentDevice(_parentDevice), refCount(1)
	{
	}

	inline LPDIRECT3DVERTEXSHADER9 GetUnderlyingVertexShader(void) const
	{
		return realObject;
	}

	inline LPDIRECT3DVERTEXDECLARATION9 GetUnderlyingVertexDeclaration(void) const
	{
		return realDecl;
	}

	virtual ~IDirect3DVertexShader8Hook()
	{
		ObjDestroy(this);
	}

	/*** IUnknown methods ***/
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE QueryInterface(THIS_ REFIID riid, void** ppvObj);
    virtual COM_DECLSPEC_NOTHROW ULONG STDMETHODCALLTYPE AddRef(THIS);
    virtual COM_DECLSPEC_NOTHROW ULONG STDMETHODCALLTYPE Release(THIS);

    /*** IDirect3DVertexShader9 methods ***/
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetDevice(THIS_ IDirect3DDevice8** ppDevice);
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetFunction(THIS_ void* pData,UINT* pSizeOfData);

	inline IDirect3DDevice8Hook* GetParentDevice(void) const
	{
		return parentDevice;
	}

protected:
	LPDIRECT3DVERTEXSHADER9 realObject;
	LPDIRECT3DVERTEXDECLARATION9 realDecl;
	IDirect3DDevice8Hook* parentDevice;
	unsigned __int64 refCount;
};
