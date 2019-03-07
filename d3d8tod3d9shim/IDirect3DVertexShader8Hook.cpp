#pragma once

#include "IDirect3DVertexShader8Hook.h"
#include "StructConverter.h"
#include "GlobalToggles.h"

/*** IUnknown methods ***/
COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DVertexShader8Hook::QueryInterface(THIS_ REFIID riid, void** ppvObj)
{
	HRESULT ret = realObject->QueryInterface(ConvertRIID8to9(riid), ppvObj);
	if (ret == NOERROR)
	{
		*ppvObj = this;
		AddRef();
	}
	return ret;
}

COM_DECLSPEC_NOTHROW ULONG STDMETHODCALLTYPE IDirect3DVertexShader8Hook::AddRef(THIS)
{
	ULONG ret = realObject ? realObject->AddRef() : 0;
	ULONG retDecl = realDecl ? realDecl->AddRef() : 0;
	++refCount;
	return ret > retDecl ? ret : retDecl;
}

COM_DECLSPEC_NOTHROW ULONG STDMETHODCALLTYPE IDirect3DVertexShader8Hook::Release(THIS)
{
	ULONG ret = realObject ? realObject->Release() : 0;
	ULONG retDecl = realDecl ? realDecl->Release() : 0;
#ifndef DISABLE_RELEASES
	if (--refCount == 0)
		delete this;
#endif
	return ret > retDecl ? ret : retDecl;
}

/*** IDirect3DVertexShader9 methods ***/
COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DVertexShader8Hook::GetDevice(THIS_ IDirect3DDevice8** ppDevice)
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

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DVertexShader8Hook::GetFunction(THIS_ void* pData,UINT* pSizeOfData)
{
	return realObject->GetFunction(pData, pSizeOfData);
}
