#pragma once

#include "IDirect3D8Hook.h"
#include "GlobalToggles.h"
#include "StructConverter.h"
#include "IDirect3DDevice8Hook.h"
#include "ObjectMap.h"

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3D8Hook::QueryInterface(THIS_ REFIID riid, void** ppvObj) 
{
	HRESULT ret = d3d9->QueryInterface(ConvertRIID8to9(riid), ppvObj);
	if (ret == NOERROR)
	{
		*ppvObj = this;
		AddRef();
	}
	return ret;
}

COM_DECLSPEC_NOTHROW ULONG STDMETHODCALLTYPE IDirect3D8Hook::AddRef(THIS) 
{
	ULONG ret = d3d9->AddRef();
	++refCount;
	return ret;
}

COM_DECLSPEC_NOTHROW ULONG STDMETHODCALLTYPE IDirect3D8Hook::Release(THIS) 
{
	ULONG ret = d3d9->Release();
#ifndef DISABLE_RELEASES
	if (--refCount == 0)
		delete this;
#endif
	return ret;
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3D8Hook::RegisterSoftwareDevice(THIS_ void* pInitializeFunction)
{
	return d3d9->RegisterSoftwareDevice(pInitializeFunction);
}

COM_DECLSPEC_NOTHROW UINT STDMETHODCALLTYPE IDirect3D8Hook::GetAdapterCount(THIS)
{
	return d3d9->GetAdapterCount();
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3D8Hook::GetAdapterIdentifier(THIS_ UINT Adapter,DWORD Flags,D3DADAPTER_IDENTIFIER8* pIdentifier)
{
	if (pIdentifier)
	{
		D3DADAPTER_IDENTIFIER9 tempIdentifier = {0};
		HRESULT ret = d3d9->GetAdapterIdentifier(Adapter, Flags, &tempIdentifier);
		ConvertD3DADAPTER_IDENTIFIER9to8(*pIdentifier, tempIdentifier);
		return ret;
	}
	return d3d9->GetAdapterIdentifier(Adapter, Flags, NULL);
}

COM_DECLSPEC_NOTHROW UINT STDMETHODCALLTYPE IDirect3D8Hook::GetAdapterModeCount(THIS_ UINT Adapter)
{
	return d3d9->GetAdapterModeCount(Adapter, DEFAULT_BACKBUFFER_FORMAT);
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3D8Hook::EnumAdapterModes(THIS_ UINT Adapter,UINT Mode,D3DDISPLAYMODE* pMode)
{
	return d3d9->EnumAdapterModes(Adapter, DEFAULT_BACKBUFFER_FORMAT, Mode, pMode);
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3D8Hook::GetAdapterDisplayMode(THIS_ UINT Adapter,D3DDISPLAYMODE* pMode)
{
	return d3d9->GetAdapterDisplayMode(Adapter, pMode);
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3D8Hook::CheckDeviceType(THIS_ UINT Adapter,D3DDEVTYPE CheckType,d3d8::D3DFORMAT DisplayFormat,d3d8::D3DFORMAT BackBufferFormat,BOOL Windowed)
{
	return d3d9->CheckDeviceType(Adapter, CheckType, ConvertD3DFORMAT_8to9(DisplayFormat), ConvertD3DFORMAT_8to9(BackBufferFormat), Windowed);
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3D8Hook::CheckDeviceFormat(THIS_ UINT Adapter,D3DDEVTYPE DeviceType,d3d8::D3DFORMAT AdapterFormat,DWORD Usage,D3DRESOURCETYPE RType,d3d8::D3DFORMAT CheckFormat)
{
	return d3d9->CheckDeviceFormat(Adapter, DeviceType, ConvertD3DFORMAT_8to9(AdapterFormat), Usage, RType, ConvertD3DFORMAT_8to9(CheckFormat) );
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3D8Hook::CheckDeviceMultiSampleType(THIS_ UINT Adapter,D3DDEVTYPE DeviceType,d3d8::D3DFORMAT SurfaceFormat,BOOL Windowed,d3d8::D3DMULTISAMPLE_TYPE MultiSampleType)
{
	// We don't care about the extra D3D9 multisample quality levels, so we can leave the last argument NULL
	D3DMULTISAMPLE_TYPE multisampleType9;
	DWORD multisampleQuality = 0;
	ConvertD3DMULTISAMPLE_TYPE8to9(MultiSampleType, multisampleType9, multisampleQuality);
	return d3d9->CheckDeviceMultiSampleType(Adapter, DeviceType, ConvertD3DFORMAT_8to9(SurfaceFormat), Windowed, multisampleType9, NULL);
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3D8Hook::CheckDepthStencilMatch(THIS_ UINT Adapter,D3DDEVTYPE DeviceType,d3d8::D3DFORMAT AdapterFormat,d3d8::D3DFORMAT RenderTargetFormat,d3d8::D3DFORMAT DepthStencilFormat)
{
	return d3d9->CheckDepthStencilMatch(Adapter, DeviceType, ConvertD3DFORMAT_8to9(AdapterFormat), ConvertD3DFORMAT_8to9(RenderTargetFormat), ConvertD3DFORMAT_8to9(DepthStencilFormat) );
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3D8Hook::GetDeviceCaps(THIS_ UINT Adapter,D3DDEVTYPE DeviceType,D3DCAPS8* pCaps)
{
	if (pCaps)
	{
		D3DCAPS9 caps9;
		ZeroMemory(&caps9, sizeof(caps9) );
		HRESULT ret = d3d9->GetDeviceCaps(Adapter, DeviceType, &caps9);
		ConvertD3DCAPS9to8(caps9, *pCaps);
		return ret;
	}
	return d3d9->GetDeviceCaps(Adapter, DeviceType, NULL);
}

COM_DECLSPEC_NOTHROW HMONITOR STDMETHODCALLTYPE IDirect3D8Hook::GetAdapterMonitor(THIS_ UINT Adapter)
{
	return d3d9->GetAdapterMonitor(Adapter);
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3D8Hook::CreateDevice(THIS_ UINT Adapter,D3DDEVTYPE DeviceType,HWND hFocusWindow,DWORD BehaviorFlags,d3d8::D3DPRESENT_PARAMETERS* pPresentationParameters,IDirect3DDevice8** ppReturnedDeviceInterface)
{
#ifdef MESSAGEBOX_ON_CREATEDEVICE
	MessageBoxA(NULL, "CreateDevice8 to 9", NULL, NULL);
#endif

	LPDIRECT3DDEVICE9 d3d9dev = NULL;
	D3DPRESENT_PARAMETERS d3dpp9 = {0};
	HRESULT ret = S_OK;
	if (pPresentationParameters)
	{
		ConvertD3DPRESENT_PARAMETERS8to9(*pPresentationParameters, d3dpp9);

#ifdef FORCE_WINDOWED_MODE
		d3dpp9.Windowed = TRUE;
		d3dpp9.FullScreen_RefreshRateInHz = 0;
#endif // FORCE_WINDOWED_MODE

		ret = d3d9->CreateDevice(Adapter, DeviceType, hFocusWindow, BehaviorFlags, &d3dpp9, &d3d9dev);
		ConvertD3DPRESENT_PARAMETERS9to8(d3dpp9, *pPresentationParameters);
	}
	else
		ret = d3d9->CreateDevice(Adapter, DeviceType, hFocusWindow, BehaviorFlags, NULL, &d3d9dev);

	if (FAILED(ret) )
		return ret;

	IDirect3DDevice8Hook* Device8 = ObjLookupCreate(d3d9dev, this);
	if (ppReturnedDeviceInterface)
		*ppReturnedDeviceInterface = Device8;

	return ret;
}

