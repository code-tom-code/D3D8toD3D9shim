#pragma once

#include "IDirect3DTexture8Hook.h"
#include "IDirect3DSurface8Hook.h"
#include "IDirect3DDevice8Hook.h"
#include "StructConverter.h"
#include "GlobalToggles.h"

/*** IUnknown methods ***/
COM_DECLSPEC_NOTHROW HRESULT IDirect3DTexture8Hook::QueryInterface(THIS_ REFIID riid, void** ppvObj)
{
	HRESULT ret = realObject->QueryInterface(ConvertRIID8to9(riid), ppvObj);
	if (ret == NOERROR)
	{
		*ppvObj = this;
		AddRef();
	}
	return ret;
}

COM_DECLSPEC_NOTHROW ULONG IDirect3DTexture8Hook::AddRef(THIS)
{
	ULONG ret = realObject->AddRef();
	++refCount;
	return ret;
}

COM_DECLSPEC_NOTHROW ULONG IDirect3DTexture8Hook::Release(THIS)
{
	ULONG ret = realObject->Release();
#ifndef DISABLE_RELEASES
	if (--refCount == 0)
		delete this;
#endif
	return ret;
}

/*** IDirect3DBaseTexture9 methods ***/
COM_DECLSPEC_NOTHROW HRESULT IDirect3DTexture8Hook::GetDevice(THIS_ IDirect3DDevice8** ppDevice)
{
	LPDIRECT3DDEVICE9 realD3D9dev = NULL;
	HRESULT ret = realObject->GetDevice(&realD3D9dev);
	if (FAILED(ret) )
	{
		*ppDevice = NULL;
		return ret;
	}

#ifdef _DEBUG
	// Check that the parentHook's underlying IDirect3DDevice9* matches the realD3D9dev pointer
	if (parentDevice->GetUnderlyingDevice() != realD3D9dev)
	{
		DbgBreakPrint("Error: Unknown d3d9 device hook detected!");
	}
#endif
	parentDevice->AddRef(); // Super important to increment the ref-count here, otherwise our parent object will get destroyed when Release() is called on it!

	*ppDevice = parentDevice;
	return ret;
}

COM_DECLSPEC_NOTHROW HRESULT IDirect3DTexture8Hook::SetPrivateData(THIS_ REFGUID refguid,CONST void* pData,DWORD SizeOfData,DWORD Flags)
{
	HRESULT ret = realObject->SetPrivateData(refguid, pData, SizeOfData, Flags);
	return ret;
}

COM_DECLSPEC_NOTHROW HRESULT IDirect3DTexture8Hook::GetPrivateData(THIS_ REFGUID refguid,void* pData,DWORD* pSizeOfData)
{
	HRESULT ret = realObject->GetPrivateData(refguid, pData, pSizeOfData);
	return ret;
}

COM_DECLSPEC_NOTHROW HRESULT IDirect3DTexture8Hook::FreePrivateData(THIS_ REFGUID refguid)
{
	HRESULT ret = realObject->FreePrivateData(refguid);
	return ret;
}

COM_DECLSPEC_NOTHROW DWORD IDirect3DTexture8Hook::SetPriority(THIS_ DWORD PriorityNew)
{
	DWORD ret = realObject->SetPriority(PriorityNew);
	return ret;
}

COM_DECLSPEC_NOTHROW DWORD IDirect3DTexture8Hook::GetPriority(THIS)
{
	DWORD ret = realObject->GetPriority();
	return ret;
}

COM_DECLSPEC_NOTHROW void IDirect3DTexture8Hook::PreLoad(THIS)
{
	realObject->PreLoad();
}

COM_DECLSPEC_NOTHROW D3DRESOURCETYPE IDirect3DTexture8Hook::GetType(THIS)
{
	return realObject->GetType();
}

COM_DECLSPEC_NOTHROW DWORD IDirect3DTexture8Hook::SetLOD(THIS_ DWORD LODNew)
{
	DWORD ret = realObject->SetLOD(LODNew);
	return ret;
}

COM_DECLSPEC_NOTHROW DWORD IDirect3DTexture8Hook::GetLOD(THIS)
{
	DWORD ret = realObject->GetLOD();
	return ret;
}

COM_DECLSPEC_NOTHROW DWORD IDirect3DTexture8Hook::GetLevelCount(THIS)
{
	DWORD ret = realObject->GetLevelCount();
	return ret;
}

COM_DECLSPEC_NOTHROW HRESULT IDirect3DTexture8Hook::GetLevelDesc(THIS_ UINT Level,d3d8::D3DSURFACE_DESC *pDesc)
{
	if (!pDesc)
		return realObject->GetLevelDesc(Level, NULL);
	D3DSURFACE_DESC desc9 = {};
	HRESULT ret = realObject->GetLevelDesc(Level, &desc9);
	ConvertD3DSURFACE_DESC9to8(desc9, *pDesc);
	return ret;
}

COM_DECLSPEC_NOTHROW HRESULT IDirect3DTexture8Hook::GetSurfaceLevel(THIS_ UINT Level,IDirect3DSurface8** ppSurfaceLevel)
{
	if (!ppSurfaceLevel)
		return realObject->GetSurfaceLevel(Level, NULL);

	LPDIRECT3DSURFACE9 surface9 = NULL;
	HRESULT ret = realObject->GetSurfaceLevel(Level, &surface9);

	IDirect3DSurface8Hook* surfacehook = ObjLookupCreate(surface9, parentDevice);
	*ppSurfaceLevel = surfacehook;

	return ret;
}

COM_DECLSPEC_NOTHROW HRESULT IDirect3DTexture8Hook::LockRect(THIS_ UINT Level,D3DLOCKED_RECT* pLockedRect,CONST RECT* pRect,DWORD Flags)
{
	return realObject->LockRect(Level, pLockedRect, pRect, Flags);
}

COM_DECLSPEC_NOTHROW HRESULT IDirect3DTexture8Hook::UnlockRect(THIS_ UINT Level)
{
	return realObject->UnlockRect(Level);
}

COM_DECLSPEC_NOTHROW HRESULT IDirect3DTexture8Hook::AddDirtyRect(THIS_ CONST RECT* pDirtyRect)
{
	return realObject->AddDirtyRect(pDirtyRect);
}
