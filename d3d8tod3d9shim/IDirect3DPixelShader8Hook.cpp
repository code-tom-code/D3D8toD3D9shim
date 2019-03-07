#pragma once

#include "IDirect3DPixelShader8Hook.h"
#include "StructConverter.h"
#include "GlobalToggles.h"

/*** IUnknown methods ***/
COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DPixelShader8Hook::QueryInterface(THIS_ REFIID riid, void** ppvObj)
{
	HRESULT ret = realObject->QueryInterface(ConvertRIID8to9(riid), ppvObj);
	if (ret == NOERROR)
	{
		*ppvObj = this;
		AddRef();
	}
	return ret;
}

COM_DECLSPEC_NOTHROW ULONG STDMETHODCALLTYPE IDirect3DPixelShader8Hook::AddRef(THIS)
{
	ULONG ret = realObject->AddRef();
	++refCount;
	return ret;
}

COM_DECLSPEC_NOTHROW ULONG STDMETHODCALLTYPE IDirect3DPixelShader8Hook::Release(THIS)
{
	ULONG ret = realObject->Release();
#ifndef DISABLE_RELEASES
	if (--refCount == 0)
		delete this;
#endif
	return ret;
}

/*** IDirect3DPixelShader9 methods ***/
COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DPixelShader8Hook::GetDevice(THIS_ IDirect3DDevice8** ppDevice)
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

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DPixelShader8Hook::GetFunction(THIS_ void* pData,UINT* pSizeOfData)
{
	if (!pData && !pSizeOfData)
		return realObject->GetFunction(NULL, NULL);

	if (!pData)
	{
		*pSizeOfData = sizeof(DWORD) * originalShaderBytecode.size();
		return D3D_OK;
	}
	else
	{
		if (pSizeOfData)
			*pSizeOfData = sizeof(DWORD) * originalShaderBytecode.size();

		DWORD* dData = (DWORD* const)pData;
		const unsigned numDWORDs = originalShaderBytecode.size();
		for (unsigned x = 0; x < numDWORDs; ++x)
			dData[x] = originalShaderBytecode[x];

		return D3D_OK;
	}
}

void IDirect3DPixelShader8Hook::SetOriginalShaderBytecode(const DWORD* pFunction)
{
	while (*pFunction != D3DSIO_END)
		originalShaderBytecode.push_back(*pFunction++);
	originalShaderBytecode.push_back(*pFunction); // Copy the END token too
}
