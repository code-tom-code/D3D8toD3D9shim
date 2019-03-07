#pragma once

#include "IDirect3DDevice8Hook.h"

struct IDirect3DBaseTexture8Hook : public IDirect3DBaseTexture8
{
public:

	IDirect3DBaseTexture8Hook(LPDIRECT3DBASETEXTURE9 _realObject, IDirect3DDevice8Hook* _parentDevice) : realObject(_realObject), parentDevice(_parentDevice), refCount(1)
	{
	}

	~IDirect3DBaseTexture8Hook()
	{
		ObjDestroy(this);
	}

	/*** IUnknown methods ***/
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE QueryInterface(THIS_ REFIID riid, void** ppvObj) override;
    virtual COM_DECLSPEC_NOTHROW ULONG STDMETHODCALLTYPE AddRef(THIS) override;
    virtual COM_DECLSPEC_NOTHROW ULONG STDMETHODCALLTYPE Release(THIS) override;

    /*** IDirect3DResource9 methods ***/
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetDevice(THIS_ IDirect3DDevice8** ppDevice) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE SetPrivateData(THIS_ REFGUID refguid,CONST void* pData,DWORD SizeOfData,DWORD Flags) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetPrivateData(THIS_ REFGUID refguid,void* pData,DWORD* pSizeOfData) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE FreePrivateData(THIS_ REFGUID refguid) override;
    virtual COM_DECLSPEC_NOTHROW DWORD STDMETHODCALLTYPE SetPriority(THIS_ DWORD PriorityNew) override;
    virtual COM_DECLSPEC_NOTHROW DWORD STDMETHODCALLTYPE GetPriority(THIS) override;
    virtual COM_DECLSPEC_NOTHROW void STDMETHODCALLTYPE PreLoad(THIS) override;
    virtual COM_DECLSPEC_NOTHROW D3DRESOURCETYPE STDMETHODCALLTYPE GetType(THIS) override;
    virtual COM_DECLSPEC_NOTHROW DWORD STDMETHODCALLTYPE SetLOD(THIS_ DWORD LODNew) override;
    virtual COM_DECLSPEC_NOTHROW DWORD STDMETHODCALLTYPE GetLOD(THIS) override;
    virtual COM_DECLSPEC_NOTHROW DWORD STDMETHODCALLTYPE GetLevelCount(THIS) override;

	inline IDirect3DDevice8Hook* GetParentDevice(void) const
	{
		return parentDevice;
	}

	inline LPDIRECT3DBASETEXTURE9 GetUnderlyingBaseTexture(void) const
	{
		return realObject;
	}

protected:
	LPDIRECT3DBASETEXTURE9 realObject;
	IDirect3DDevice8Hook* parentDevice;
	unsigned __int64 refCount;
};
