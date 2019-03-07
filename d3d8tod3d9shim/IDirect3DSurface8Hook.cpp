#pragma once

#include "GlobalToggles.h"
#include "IDirect3DSurface8Hook.h"
#include "StructConverter.h"
#include "GlobalToggles.h"

/*** IUnknown methods ***/
COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DSurface8Hook::QueryInterface(THIS_ REFIID riid, void** ppvObj)
{
	HRESULT ret = realObject->QueryInterface(ConvertRIID8to9(riid), ppvObj);
	if (ret == NOERROR)
	{
		*ppvObj = this;
		AddRef();
	}
	return ret;
}

COM_DECLSPEC_NOTHROW ULONG STDMETHODCALLTYPE IDirect3DSurface8Hook::AddRef(THIS)
{
	ULONG ret = realObject->AddRef();
	++refCount;
	return ret;
}

COM_DECLSPEC_NOTHROW ULONG STDMETHODCALLTYPE IDirect3DSurface8Hook::Release(THIS)
{
	ULONG ret = realObject->Release();
#ifndef DISABLE_RELEASES
	if (--refCount == 0)
		delete this;
#endif
	return ret;
}

/*** IDirect3DResource9 methods ***/
COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DSurface8Hook::GetDevice(THIS_ IDirect3DDevice8** ppDevice)
{
	LPDIRECT3DDEVICE9 realD3D9dev = NULL;
	HRESULT ret = realObject->GetDevice(&realD3D9dev);
	if (FAILED(ret) )
	{
		*ppDevice = NULL;
		return ret;
	}

	// Check that the parentHook's underlying IDirect3DDevice9* matches the realD3D9dev pointer
	if (parentDevice->GetUnderlyingDevice() != realD3D9dev)
	{
		DbgBreakPrint("Error: Unknown d3d9 device hook detected!");
	}
	parentDevice->AddRef(); // Super important to increment the ref-count here, otherwise our parent object will get destroyed when Release() is called on it!

	*ppDevice = parentDevice;
	return ret;
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DSurface8Hook::SetPrivateData(THIS_ REFGUID refguid,CONST void* pData,DWORD SizeOfData,DWORD Flags)
{
	HRESULT ret = realObject->SetPrivateData(refguid, pData, SizeOfData, Flags);
	return ret;
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DSurface8Hook::GetPrivateData(THIS_ REFGUID refguid,void* pData,DWORD* pSizeOfData)
{
	HRESULT ret = realObject->GetPrivateData(refguid, pData, pSizeOfData);
	return ret;
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DSurface8Hook::FreePrivateData(THIS_ REFGUID refguid)
{
	HRESULT ret = realObject->FreePrivateData(refguid);
	return ret;
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DSurface8Hook::GetContainer(THIS_ REFIID riid,void** ppContainer)
{
	if (!ppContainer)
		return realObject->GetContainer(riid, NULL);

	IUnknown* d3d9container = NULL;
	HRESULT ret = realObject->GetContainer(riid, (void**)&d3d9container);
	if (FAILED(ret) )
		return ret;

	if (!d3d9container)
	{
		*ppContainer = NULL;
		return ret;
	}

	LPDIRECT3DDEVICE9 devicecontainer = NULL;
	LPDIRECT3DSWAPCHAIN9 swapchaincontainer = NULL;
	LPDIRECT3DTEXTURE9 texturecontainer = NULL;
	LPDIRECT3D9 deviceparent = NULL;
	LPDIRECT3DDEVICE9 device = NULL;
	IDirect3D8Hook* deviceparenthook = NULL;
	IDirect3DDevice8Hook* devicehook = NULL;
	if (d3d9container->QueryInterface(IID_IDirect3DDevice9, (void**)&devicecontainer) == NOERROR && devicecontainer)
	{
		devicecontainer->GetDirect3D(&deviceparent);
		deviceparenthook = ObjLookupCreate(deviceparent);
		devicehook = ObjLookupCreate(devicecontainer, deviceparenthook);
		*ppContainer = devicehook;
	}
	else if (d3d9container->QueryInterface(IID_IDirect3DSwapChain9, (void**)&swapchaincontainer) == NOERROR && swapchaincontainer)
	{
		swapchaincontainer->GetDevice(&device);
		device->GetDirect3D(&deviceparent);
		deviceparenthook = ObjLookupCreate(deviceparent);
		devicehook = ObjLookupCreate(device, deviceparenthook);
		IDirect3DSwapChain8Hook* swapchainhook = ObjLookupCreate(swapchaincontainer, devicehook);
		*ppContainer = swapchainhook;
	}
	else if (d3d9container->QueryInterface(IID_IDirect3DTexture9, (void**)&texturecontainer) == NOERROR && texturecontainer)
	{
		texturecontainer->GetDevice(&device);
		device->GetDirect3D(&deviceparent);
		deviceparenthook = ObjLookupCreate(deviceparent);
		devicehook = ObjLookupCreate(device, deviceparenthook);
		IDirect3DTexture8Hook* texturehook = ObjLookupCreate(texturecontainer, devicehook);
		*ppContainer = texturehook;
	}
	else
	{
		// We probably shouldn't ever be here because I'm not sure what else is allowed to own a Surface in D3D9...
		__debugbreak();

		*ppContainer = NULL;
		return ret;
	}

	return ret;
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DSurface8Hook::GetDesc(THIS_ d3d8::D3DSURFACE_DESC *pDesc)
{
	if (!pDesc)
		return realObject->GetDesc(NULL);

	D3DSURFACE_DESC desc9 = {};
	HRESULT ret = realObject->GetDesc(&desc9);
	ConvertD3DSURFACE_DESC9to8(desc9, *pDesc);
	return ret;
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DSurface8Hook::LockRect(THIS_ D3DLOCKED_RECT* pLockedRect,CONST RECT* pRect,DWORD Flags)
{
	return realObject->LockRect(pLockedRect, pRect, Flags);
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DSurface8Hook::UnlockRect(THIS)
{
	return realObject->UnlockRect();
}
