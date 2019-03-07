#pragma once

#include "IDirect3DDevice8Hook.h"

struct IDirect3DCubeTexture8Hook : public IDirect3DCubeTexture8
{
public:
	IDirect3DCubeTexture8Hook(LPDIRECT3DCUBETEXTURE9 _realObject, IDirect3DDevice8Hook* _parentDevice) : realObject(_realObject), parentDevice(_parentDevice), refCount(1)
	{
	}

	~IDirect3DCubeTexture8Hook()
	{
		ObjDestroy(this);
	}

	/*** IUnknown methods ***/
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE QueryInterface(THIS_ REFIID riid, void** ppvObj) override;
    virtual COM_DECLSPEC_NOTHROW ULONG STDMETHODCALLTYPE AddRef(THIS) override;
    virtual COM_DECLSPEC_NOTHROW ULONG STDMETHODCALLTYPE Release(THIS) override;

    /*** IDirect3DBaseTexture9 methods ***/
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
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetLevelDesc(THIS_ UINT Level,d3d8::D3DSURFACE_DESC *pDesc) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetCubeMapSurface(THIS_ D3DCUBEMAP_FACES FaceType,UINT Level,IDirect3DSurface8** ppCubeMapSurface) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE LockRect(THIS_ D3DCUBEMAP_FACES FaceType,UINT Level,D3DLOCKED_RECT* pLockedRect,CONST RECT* pRect,DWORD Flags) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE UnlockRect(THIS_ D3DCUBEMAP_FACES FaceType,UINT Level) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE AddDirtyRect(THIS_ D3DCUBEMAP_FACES FaceType,CONST RECT* pDirtyRect) override;

	inline LPDIRECT3DCUBETEXTURE9 GetUnderlyingCubeTexture(void) const
	{
		return realObject;
	}

	inline IDirect3DDevice8Hook* GetParentDevice(void) const
	{
		return parentDevice;
	}

protected:
	LPDIRECT3DCUBETEXTURE9 realObject;
	IDirect3DDevice8Hook* parentDevice;
	unsigned __int64 refCount;
};
