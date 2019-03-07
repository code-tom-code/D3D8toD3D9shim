#pragma once

#include "IDirect3DVolumeTexture8Hook.h"
#include "IDirect3DVolume8Hook.h"
#include "StructConverter.h"
#include "GlobalToggles.h"

/*** IUnknown methods ***/
COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DVolumeTexture8Hook::QueryInterface(THIS_ REFIID riid, void** ppvObj)
{
	HRESULT ret = realObject->QueryInterface(ConvertRIID8to9(riid), ppvObj);
	if (ret == NOERROR)
	{
		*ppvObj = this;
		AddRef();
	}
	return ret;
}

COM_DECLSPEC_NOTHROW ULONG STDMETHODCALLTYPE IDirect3DVolumeTexture8Hook::AddRef(THIS)
{
	ULONG ret = realObject->AddRef();
	++refCount;
	return ret;
}

COM_DECLSPEC_NOTHROW ULONG STDMETHODCALLTYPE IDirect3DVolumeTexture8Hook::Release(THIS)
{
	ULONG ret = realObject->Release();
#ifndef DISABLE_RELEASES
	if (--refCount == 0)
		delete this;
#endif
	return ret;
}


/*** IDirect3DBaseTexture9 methods ***/
COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DVolumeTexture8Hook::GetDevice(THIS_ IDirect3DDevice8** ppDevice)
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

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DVolumeTexture8Hook::SetPrivateData(THIS_ REFGUID refguid,CONST void* pData,DWORD SizeOfData,DWORD Flags)
{
	HRESULT ret = realObject->SetPrivateData(refguid, pData, SizeOfData, Flags);
	return ret;
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DVolumeTexture8Hook::GetPrivateData(THIS_ REFGUID refguid,void* pData,DWORD* pSizeOfData)
{
	HRESULT ret = realObject->GetPrivateData(refguid, pData, pSizeOfData);
	return ret;
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DVolumeTexture8Hook::FreePrivateData(THIS_ REFGUID refguid)
{
	HRESULT ret = realObject->FreePrivateData(refguid);
	return ret;
}

COM_DECLSPEC_NOTHROW DWORD STDMETHODCALLTYPE IDirect3DVolumeTexture8Hook::SetPriority(THIS_ DWORD PriorityNew)
{
	DWORD ret = realObject->SetPriority(PriorityNew);
	return ret;
}

COM_DECLSPEC_NOTHROW DWORD STDMETHODCALLTYPE IDirect3DVolumeTexture8Hook::GetPriority(THIS)
{
	DWORD ret = realObject->GetPriority();
	return ret;
}

COM_DECLSPEC_NOTHROW void STDMETHODCALLTYPE IDirect3DVolumeTexture8Hook::PreLoad(THIS)
{
	realObject->PreLoad();
}

COM_DECLSPEC_NOTHROW D3DRESOURCETYPE STDMETHODCALLTYPE IDirect3DVolumeTexture8Hook::GetType(THIS)
{
	D3DRESOURCETYPE ret = realObject->GetType();
	return ret;
}

COM_DECLSPEC_NOTHROW DWORD STDMETHODCALLTYPE IDirect3DVolumeTexture8Hook::SetLOD(THIS_ DWORD LODNew)
{
	DWORD ret = realObject->SetLOD(LODNew);
	return ret;
}

COM_DECLSPEC_NOTHROW DWORD STDMETHODCALLTYPE IDirect3DVolumeTexture8Hook::GetLOD(THIS)
{
	DWORD ret = realObject->GetLOD();
	return ret;
}

COM_DECLSPEC_NOTHROW DWORD STDMETHODCALLTYPE IDirect3DVolumeTexture8Hook::GetLevelCount(THIS)
{
	DWORD ret = realObject->GetLevelCount();
	return ret;
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DVolumeTexture8Hook::GetLevelDesc(THIS_ UINT Level,d3d8::D3DVOLUME_DESC *pDesc)
{
	if (!pDesc)
		return realObject->GetLevelDesc(Level, NULL);
	D3DVOLUME_DESC desc9 = {};
	HRESULT ret = realObject->GetLevelDesc(Level, &desc9);
	ConvertD3DVOLUME_DESC9to8(desc9, *pDesc);
	return ret;
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DVolumeTexture8Hook::GetVolumeLevel(THIS_ UINT Level,IDirect3DVolume8** ppVolumeLevel)
{
	if (!ppVolumeLevel)
		return realObject->GetVolumeLevel(Level, NULL);

	LPDIRECT3DVOLUME9 volume9 = NULL;
	HRESULT ret = realObject->GetVolumeLevel(Level, &volume9);

	IDirect3DVolume8Hook* volumehook = ObjLookupCreate(volume9, parentDevice);
	*ppVolumeLevel = volumehook;

	return ret;
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DVolumeTexture8Hook::LockBox(THIS_ UINT Level,D3DLOCKED_BOX* pLockedVolume,CONST D3DBOX* pBox,DWORD Flags)
{
	HRESULT ret = realObject->LockBox(Level, pLockedVolume, pBox, Flags);
	return ret;
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DVolumeTexture8Hook::UnlockBox(THIS_ UINT Level)
{
	HRESULT ret = realObject->UnlockBox(Level);
	return ret;
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DVolumeTexture8Hook::AddDirtyBox(THIS_ CONST D3DBOX* pDirtyBox)
{
	HRESULT ret = realObject->AddDirtyBox(pDirtyBox);
	return ret;
}
