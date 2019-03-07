#pragma once

#include "IDirect3DDevice8Hook.h"

struct IDirect3DVolume8Hook : public IDirect3DVolume8
{
public:
	IDirect3DVolume8Hook(LPDIRECT3DVOLUME9 _realObject, IDirect3DDevice8Hook* _parentDevice) : realObject(_realObject), parentDevice(_parentDevice), refCount(1)
	{
	}

	~IDirect3DVolume8Hook()
	{
		ObjDestroy(this);
	}

	/*** IUnknown methods ***/
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE QueryInterface(THIS_ REFIID riid, void** ppvObj) override;
    virtual COM_DECLSPEC_NOTHROW ULONG STDMETHODCALLTYPE AddRef(THIS) override;
    virtual COM_DECLSPEC_NOTHROW ULONG STDMETHODCALLTYPE Release(THIS) override;

    /*** IDirect3DVolume9 methods ***/
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetDevice(THIS_ IDirect3DDevice8** ppDevice) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE SetPrivateData(THIS_ REFGUID refguid,CONST void* pData,DWORD SizeOfData,DWORD Flags) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetPrivateData(THIS_ REFGUID refguid,void* pData,DWORD* pSizeOfData) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE FreePrivateData(THIS_ REFGUID refguid) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetContainer(THIS_ REFIID riid,void** ppContainer) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetDesc(THIS_ d3d8::D3DVOLUME_DESC *pDesc) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE LockBox(THIS_ D3DLOCKED_BOX * pLockedVolume,CONST D3DBOX* pBox,DWORD Flags) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE UnlockBox(THIS) override;

	inline IDirect3DDevice8Hook* GetParentDevice(void) const
	{
		return parentDevice;
	}

	inline LPDIRECT3DVOLUME9 GetUnderlyingVolume(void) const
	{
		return realObject;
	}

protected:
	LPDIRECT3DVOLUME9 realObject;
	IDirect3DDevice8Hook* parentDevice;
	unsigned __int64 refCount;
};
