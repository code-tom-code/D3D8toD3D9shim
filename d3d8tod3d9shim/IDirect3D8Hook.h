#pragma once

#include "d3d9include.h"
#include "d3d8include.h"

#include <stddef.h> // for offsetof

#include "IDirect3DDevice8Hook.h"
#include "ObjectMap.h"
#include <map>

struct IDirect3DDevice8Hook;

struct IDirect3D8Hook : public IDirect3D8
{
public:
	IDirect3D8Hook(LPDIRECT3D9 _d3d9) : d3d9(_d3d9), refCount(1)
	{
		InitializeCriticalSection(&cs);
	}

	virtual ~IDirect3D8Hook()
	{
		DeleteCriticalSection(&cs);
		ObjDestroy(this);
	}

	/*** IUnknown methods ***/
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE QueryInterface(THIS_ REFIID riid, void** ppvObj) override;
    virtual COM_DECLSPEC_NOTHROW ULONG STDMETHODCALLTYPE AddRef(THIS) override;
    virtual COM_DECLSPEC_NOTHROW ULONG STDMETHODCALLTYPE Release(THIS) override;

    /*** IDirect3D8 methods ***/
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE RegisterSoftwareDevice(THIS_ void* pInitializeFunction) override;
    virtual COM_DECLSPEC_NOTHROW UINT STDMETHODCALLTYPE GetAdapterCount(THIS) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetAdapterIdentifier(THIS_ UINT Adapter,DWORD Flags,D3DADAPTER_IDENTIFIER8* pIdentifier) override;
    virtual COM_DECLSPEC_NOTHROW UINT STDMETHODCALLTYPE GetAdapterModeCount(THIS_ UINT Adapter) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE EnumAdapterModes(THIS_ UINT Adapter,UINT Mode,D3DDISPLAYMODE* pMode) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetAdapterDisplayMode(THIS_ UINT Adapter,D3DDISPLAYMODE* pMode) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE CheckDeviceType(THIS_ UINT Adapter,D3DDEVTYPE CheckType,d3d8::D3DFORMAT DisplayFormat,d3d8::D3DFORMAT BackBufferFormat,BOOL Windowed) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE CheckDeviceFormat(THIS_ UINT Adapter,D3DDEVTYPE DeviceType,d3d8::D3DFORMAT AdapterFormat,DWORD Usage,D3DRESOURCETYPE RType,d3d8::D3DFORMAT CheckFormat) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE CheckDeviceMultiSampleType(THIS_ UINT Adapter,D3DDEVTYPE DeviceType,d3d8::D3DFORMAT SurfaceFormat,BOOL Windowed,d3d8::D3DMULTISAMPLE_TYPE MultiSampleType) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE CheckDepthStencilMatch(THIS_ UINT Adapter,D3DDEVTYPE DeviceType,d3d8::D3DFORMAT AdapterFormat,d3d8::D3DFORMAT RenderTargetFormat,d3d8::D3DFORMAT DepthStencilFormat) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetDeviceCaps(THIS_ UINT Adapter,D3DDEVTYPE DeviceType,D3DCAPS8* pCaps) override;
    virtual COM_DECLSPEC_NOTHROW HMONITOR STDMETHODCALLTYPE GetAdapterMonitor(THIS_ UINT Adapter) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE CreateDevice(THIS_ UINT Adapter,D3DDEVTYPE DeviceType,HWND hFocusWindow,DWORD BehaviorFlags,d3d8::D3DPRESENT_PARAMETERS* pPresentationParameters,IDirect3DDevice8** ppReturnedDeviceInterface) override;

	LPDIRECT3D9 d3d9;

	CRITICAL_SECTION cs;

	// Don't be a jerk and access this map without locking the critical section first!
	std::map<IDirect3DDevice9*, IDirect3DDevice8Hook*> Devices;

protected:
	unsigned __int64 refCount;
};
