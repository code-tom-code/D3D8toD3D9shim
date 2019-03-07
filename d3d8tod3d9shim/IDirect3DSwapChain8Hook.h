#pragma once

#include "IDirect3DDevice8Hook.h"

struct IDirect3DSwapChain8Hook : public IDirect3DSwapChain8
{
public:
	IDirect3DSwapChain8Hook(LPDIRECT3DSWAPCHAIN9 _realObject, IDirect3DDevice8Hook* _parentDevice) : realObject(_realObject), parentDevice(_parentDevice), refCount(1)
	{
	}

	~IDirect3DSwapChain8Hook()
	{
		ObjDestroy(this);
	}

	void InitializeSwapChain(const D3DPRESENT_PARAMETERS& _PresentParameters, IDirect3DSurface8Hook* swapChainBackBuffer);

	/*** IUnknown methods ***/
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE QueryInterface(THIS_ REFIID riid, void** ppvObj) override;
    virtual COM_DECLSPEC_NOTHROW ULONG STDMETHODCALLTYPE AddRef(THIS) override;
    virtual COM_DECLSPEC_NOTHROW ULONG STDMETHODCALLTYPE Release(THIS) override;

    /*** IDirect3DSwapChain9 methods ***/
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE Present(THIS_ CONST RECT* pSourceRect,CONST RECT* pDestRect,HWND hDestWindowOverride,CONST RGNDATA* pDirtyRegion) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetBackBuffer(THIS_ UINT iBackBuffer,D3DBACKBUFFER_TYPE Type,IDirect3DSurface8** ppBackBuffer) override;

	inline LPDIRECT3DSWAPCHAIN9 GetUnderlyingSwapChain(void) const
	{
		return realObject;
	}

	inline IDirect3DDevice8Hook* GetParentDevice(void) const
	{
		return parentDevice;
	}

protected:
	LPDIRECT3DSWAPCHAIN9 realObject;
	IDirect3DDevice8Hook* parentDevice;
	unsigned __int64 refCount;
};
