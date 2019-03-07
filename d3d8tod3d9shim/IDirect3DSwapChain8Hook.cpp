#pragma once

#include "IDirect3DSwapChain8Hook.h"
#include "IDirect3DSurface8Hook.h"
#include "StructConverter.h"
#include "GlobalToggles.h"

/*** IUnknown methods ***/
COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DSwapChain8Hook::QueryInterface(THIS_ REFIID riid, void** ppvObj)
{
	HRESULT ret = realObject->QueryInterface(ConvertRIID8to9(riid), ppvObj);
	if (ret == NOERROR)
	{
		*ppvObj = this;
		AddRef();
	}
	return ret;
}

COM_DECLSPEC_NOTHROW ULONG STDMETHODCALLTYPE IDirect3DSwapChain8Hook::AddRef(THIS)
{
	ULONG ret = realObject->AddRef();
	++refCount;
	return ret;
}

COM_DECLSPEC_NOTHROW ULONG STDMETHODCALLTYPE IDirect3DSwapChain8Hook::Release(THIS)
{
	ULONG ret = realObject->Release();
#ifndef DISABLE_RELEASES
	if (--refCount == 0)
		delete this;
#endif
	return ret;
}

/*** IDirect3DSwapChain9 methods ***/
COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DSwapChain8Hook::Present(THIS_ CONST RECT* pSourceRect,CONST RECT* pDestRect,HWND hDestWindowOverride,CONST RGNDATA* pDirtyRegion)
{
	return realObject->Present(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion, 0);
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DSwapChain8Hook::GetBackBuffer(THIS_ UINT iBackBuffer,D3DBACKBUFFER_TYPE Type,IDirect3DSurface8** ppBackBuffer)
{
	if (!ppBackBuffer)
		return realObject->GetBackBuffer(iBackBuffer, Type, NULL);

	LPDIRECT3DSURFACE9 surface9 = NULL;
	HRESULT ret = realObject->GetBackBuffer(iBackBuffer, Type, &surface9);

	IDirect3DSurface8Hook* surfacehook = ObjLookupCreate(surface9, parentDevice);
	*ppBackBuffer = surfacehook;

	return ret;
}
