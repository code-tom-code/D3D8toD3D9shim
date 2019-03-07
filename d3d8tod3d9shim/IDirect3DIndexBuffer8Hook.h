#pragma once

#include "IDirect3DDevice8Hook.h"

struct IDirect3DIndexBuffer8Hook : public IDirect3DIndexBuffer8
{
public:
	IDirect3DIndexBuffer8Hook(LPDIRECT3DINDEXBUFFER9 _realObject, IDirect3DDevice8Hook* _parentDevice) : realObject(_realObject), parentDevice(_parentDevice), refCount(1)
	{
	}

	inline LPDIRECT3DINDEXBUFFER9 GetUnderlyingIndexBuffer(void) const
	{
		return realObject;
	}

	virtual ~IDirect3DIndexBuffer8Hook()
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
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE Lock(THIS_ UINT OffsetToLock,UINT SizeToLock,BYTE** ppbData,DWORD Flags) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE Unlock(THIS) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetDesc(THIS_ D3DINDEXBUFFER_DESC *pDesc) override;

	inline IDirect3DDevice8Hook* GetParentDevice(void) const
	{
		return parentDevice;
	}

protected:
	LPDIRECT3DINDEXBUFFER9 realObject;
	IDirect3DDevice8Hook* parentDevice;
	unsigned __int64 refCount;
};
