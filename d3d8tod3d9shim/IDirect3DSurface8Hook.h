#pragma once

#include "IDirect3DDevice8Hook.h"
#include "IDirect3DResource8Hook.h"

struct IDirect3DSurface8Hook : public IDirect3DSurface8
{
public:
	IDirect3DSurface8Hook(LPDIRECT3DSURFACE9 _realObject, IDirect3DDevice8Hook* _parentDevice) : realObject(_realObject), parentDevice(_parentDevice), refCount(1)
	{
	}

	~IDirect3DSurface8Hook()
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
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetContainer(THIS_ REFIID riid,void** ppContainer) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetDesc(THIS_ d3d8::D3DSURFACE_DESC *pDesc) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE LockRect(THIS_ D3DLOCKED_RECT* pLockedRect,CONST RECT* pRect,DWORD Flags) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE UnlockRect(THIS) override;

	inline IDirect3DDevice8Hook* GetParentDevice(void) const
	{
		return parentDevice;
	}

	inline LPDIRECT3DSURFACE9 GetUnderlyingSurface(void) const
	{
		return realObject;
	}

protected:
	LPDIRECT3DSURFACE9 realObject;
	IDirect3DDevice8Hook* parentDevice;
	unsigned __int64 refCount;
};
