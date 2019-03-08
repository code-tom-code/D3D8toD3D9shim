#pragma once

#include "GlobalToggles.h"
#include "IDirect3DDevice8Hook.h"
#include "IDirect3D8Hook.h"
#include "StructConverter.h"
#include "IDirect3DIndexBuffer8Hook.h"
#include "IDirect3DVertexBuffer8Hook.h"
#include "IDirect3DVertexShader8Hook.h"
#include "IDirect3DPixelShader8Hook.h"
#include "IDirect3DSurface8Hook.h"
#include "IDirect3DSwapChain8Hook.h"
#include "IDirect3DBaseTexture8Hook.h"
#include "IDirect3DTexture8Hook.h"
#include "IDirect3DCubeTexture8Hook.h"
#include "IDirect3DVolumeTexture8Hook.h"
#include "IDirect3DStateBlock8Hook.h"
#include <vector>
#include "ShaderAnalysis.h"

/*** IUnknown methods ***/
COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DDevice8Hook::QueryInterface(THIS_ REFIID riid, void** ppvObj) 
{
	HRESULT ret = d3d9dev->QueryInterface(ConvertRIID8to9(riid), ppvObj);
	if (ret == NOERROR)
	{
		*ppvObj = this;
		AddRef();
	}
	return ret;
}

COM_DECLSPEC_NOTHROW ULONG STDMETHODCALLTYPE IDirect3DDevice8Hook::AddRef(THIS)
{
	ULONG ret = d3d9dev->AddRef();
	++refCount;
	return ret;
}

COM_DECLSPEC_NOTHROW ULONG STDMETHODCALLTYPE IDirect3DDevice8Hook::Release(THIS)
{
	ULONG ret = d3d9dev->Release();
#ifndef DISABLE_RELEASES
	if (--refCount == 0)
	{
#ifdef _DEBUG
		for (unsigned x = 0; x < ARRAYSIZE(magicVal); ++x)
		{
			if (magicVal[x] != 0xFFFFFFFF - x)
			{
				__debugbreak();
			}
		}
#endif
		delete this;
	}
#endif
	return ret;
}

/*** IDirect3DDevice9 methods ***/
COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DDevice8Hook::TestCooperativeLevel(THIS)
{
	return d3d9dev->TestCooperativeLevel();
}

COM_DECLSPEC_NOTHROW UINT STDMETHODCALLTYPE IDirect3DDevice8Hook::GetAvailableTextureMem(THIS)
{
	return d3d9dev->GetAvailableTextureMem();
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DDevice8Hook::GetDirect3D(THIS_ IDirect3D8** ppD3D8)
{
	if (!ppD3D8)
		return d3d9dev->GetDirect3D(NULL);
	*ppD3D8 = parentHook;
	parentHook->AddRef();
	return S_OK;
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DDevice8Hook::GetDeviceCaps(THIS_ D3DCAPS8* pCaps)
{
	if (!pCaps)
		return d3d9dev->GetDeviceCaps(NULL);
	D3DCAPS9 caps9;
	ZeroMemory(&caps9, sizeof(caps9) );
	HRESULT ret = d3d9dev->GetDeviceCaps(&caps9);
	ConvertD3DCAPS9to8(caps9, *pCaps);
	return ret;
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DDevice8Hook::GetDisplayMode(THIS_ D3DDISPLAYMODE* pMode)
{
	return d3d9dev->GetDisplayMode(0, pMode);
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DDevice8Hook::GetCreationParameters(THIS_ D3DDEVICE_CREATION_PARAMETERS* pParameters)
{
	return d3d9dev->GetCreationParameters(pParameters);
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DDevice8Hook::SetCursorProperties(THIS_ UINT XHotSpot, UINT YHotSpot, IDirect3DSurface8* pCursorBitmap)
{
	if (!pCursorBitmap)
		return d3d9dev->SetCursorProperties(XHotSpot, YHotSpot, NULL);

	IDirect3DSurface8Hook* surfacehook = dynamic_cast<IDirect3DSurface8Hook*>(pCursorBitmap);
	if (!surfacehook)
	{
		__debugbreak();
	}

	return d3d9dev->SetCursorProperties(XHotSpot, YHotSpot, surfacehook->GetUnderlyingSurface() );
}

COM_DECLSPEC_NOTHROW void STDMETHODCALLTYPE IDirect3DDevice8Hook::SetCursorPosition(THIS_ int X, int Y, DWORD Flags)
{
	d3d9dev->SetCursorPosition(X, Y, Flags);
}

COM_DECLSPEC_NOTHROW BOOL STDMETHODCALLTYPE IDirect3DDevice8Hook::ShowCursor(THIS_ BOOL bShow)
{
	return d3d9dev->ShowCursor(bShow);
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DDevice8Hook::CreateAdditionalSwapChain(THIS_ d3d8::D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DSwapChain8** pSwapChain)
{
	if (!pPresentationParameters)
	{
		LPDIRECT3DSWAPCHAIN9 swap9 = NULL;
		HRESULT ret = d3d9dev->CreateAdditionalSwapChain(NULL, &swap9);
		IDirect3DSwapChain8Hook* swaphook = ObjLookupCreate(swap9, this);
		*pSwapChain = swaphook;
		return ret;
	}

	D3DPRESENT_PARAMETERS d3dpp9 = {0};
	ConvertD3DPRESENT_PARAMETERS8to9(*pPresentationParameters, d3dpp9);
	LPDIRECT3DSWAPCHAIN9 swap9 = NULL;
	HRESULT ret = d3d9dev->CreateAdditionalSwapChain(&d3dpp9, &swap9);
	IDirect3DSwapChain8Hook* swaphook = ObjLookupCreate(swap9, this);
	*pSwapChain = swaphook;
	ConvertD3DPRESENT_PARAMETERS9to8(d3dpp9, *pPresentationParameters);
	return ret;
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DDevice8Hook::Reset(THIS_ d3d8::D3DPRESENT_PARAMETERS* pPresentationParameters)
{
#ifdef _DEBUG
	MessageBoxA(NULL, "Reset", NULL, NULL);
#endif

	if (!pPresentationParameters)
		return d3d9dev->Reset(NULL);

	D3DPRESENT_PARAMETERS d3dpp9 = {0};
	ConvertD3DPRESENT_PARAMETERS8to9(*pPresentationParameters, d3dpp9);

#ifdef FORCE_WINDOWED_MODE
	d3dpp9.Windowed = TRUE;
	d3dpp9.FullScreen_RefreshRateInHz = 0;
#endif // FORCE_WINDOWED_MODE

	HRESULT ret = d3d9dev->Reset(&d3dpp9);
	ConvertD3DPRESENT_PARAMETERS9to8(d3dpp9, *pPresentationParameters);
	return ret;
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DDevice8Hook::Present(THIS_ CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion)
{
	return d3d9dev->Present(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DDevice8Hook::GetBackBuffer(THIS_ UINT iBackBuffer, D3DBACKBUFFER_TYPE Type, IDirect3DSurface8** ppBackBuffer)
{
	if (!ppBackBuffer)
		return d3d9dev->GetBackBuffer(0, iBackBuffer, Type, NULL);

	LPDIRECT3DSURFACE9 surface9 = NULL;
	HRESULT ret = d3d9dev->GetBackBuffer(0, iBackBuffer, Type, &surface9);

	IDirect3DSurface8Hook* surfacehook = ObjLookupCreate(surface9, this);
	*ppBackBuffer = surfacehook;

	return ret;
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DDevice8Hook::GetRasterStatus(THIS_ D3DRASTER_STATUS* pRasterStatus)
{
	return d3d9dev->GetRasterStatus(0, pRasterStatus);
}

COM_DECLSPEC_NOTHROW void STDMETHODCALLTYPE IDirect3DDevice8Hook::SetGammaRamp(THIS_ DWORD Flags, CONST D3DGAMMARAMP* pRamp)
{
	d3d9dev->SetGammaRamp(0, Flags, pRamp);
}

COM_DECLSPEC_NOTHROW void STDMETHODCALLTYPE IDirect3DDevice8Hook::GetGammaRamp(THIS_ D3DGAMMARAMP* pRamp)
{
	d3d9dev->GetGammaRamp(0, pRamp);
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DDevice8Hook::CreateTexture(THIS_ UINT Width, UINT Height, UINT Levels, DWORD Usage, d3d8::D3DFORMAT Format, D3DPOOL Pool, IDirect3DTexture8** ppTexture)
{
	if (!ppTexture)
		return d3d9dev->CreateTexture(Width, Height, Levels, Usage, ConvertD3DFORMAT_8to9(Format), Pool, NULL, NULL);

	LPDIRECT3DTEXTURE9 tex9 = NULL;
	HRESULT ret = d3d9dev->CreateTexture(Width, Height, Levels, Usage, ConvertD3DFORMAT_8to9(Format), Pool, &tex9, NULL);

	IDirect3DTexture8Hook* texhook = ObjLookupCreate(tex9, this);
	*ppTexture = texhook;

	return ret;
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DDevice8Hook::CreateVolumeTexture(THIS_ UINT Width, UINT Height, UINT Depth, UINT Levels, DWORD Usage, d3d8::D3DFORMAT Format, D3DPOOL Pool, IDirect3DVolumeTexture8** ppVolumeTexture)
{
	if (!ppVolumeTexture)
		return d3d9dev->CreateVolumeTexture(Width, Height, Depth, Levels, Usage, ConvertD3DFORMAT_8to9(Format), Pool, NULL, NULL);

	LPDIRECT3DVOLUMETEXTURE9 tex9 = NULL;
	HRESULT ret = d3d9dev->CreateVolumeTexture(Width, Height, Depth, Levels, Usage, ConvertD3DFORMAT_8to9(Format), Pool, &tex9, NULL);

	IDirect3DVolumeTexture8Hook* texhook = ObjLookupCreate(tex9, this);
	*ppVolumeTexture = texhook;

	return ret;
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DDevice8Hook::CreateCubeTexture(THIS_ UINT EdgeLength, UINT Levels, DWORD Usage, d3d8::D3DFORMAT Format, D3DPOOL Pool, IDirect3DCubeTexture8** ppCubeTexture)
{
	if (!ppCubeTexture)
		return d3d9dev->CreateCubeTexture(EdgeLength, Levels, Usage, ConvertD3DFORMAT_8to9(Format), Pool, NULL, NULL);

	LPDIRECT3DCUBETEXTURE9 tex9 = NULL;
	HRESULT ret = d3d9dev->CreateCubeTexture(EdgeLength, Levels, Usage, ConvertD3DFORMAT_8to9(Format), Pool, &tex9, NULL);

	IDirect3DCubeTexture8Hook* texhook = ObjLookupCreate(tex9, this);
	*ppCubeTexture = texhook;

	return ret;
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DDevice8Hook::CreateVertexBuffer(THIS_ UINT Length, DWORD Usage, DWORD FVF, D3DPOOL Pool, IDirect3DVertexBuffer8** ppVertexBuffer)
{
	if (!ppVertexBuffer)
		return d3d9dev->CreateVertexBuffer(Length, Usage, FVF, Pool, NULL, NULL);

	LPDIRECT3DVERTEXBUFFER9 vb9 = NULL;
	HRESULT ret = d3d9dev->CreateVertexBuffer(Length, Usage, FVF, Pool, &vb9, NULL);

	IDirect3DVertexBuffer8Hook* vbhook = ObjLookupCreate(vb9, this);
	*ppVertexBuffer = vbhook;

	return ret;
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DDevice8Hook::CreateIndexBuffer(THIS_ UINT Length, DWORD Usage, d3d8::D3DFORMAT Format, D3DPOOL Pool, IDirect3DIndexBuffer8** ppIndexBuffer)
{
	if (!ppIndexBuffer)
		return d3d9dev->CreateIndexBuffer(Length, Usage, ConvertD3DFORMAT_8to9(Format), Pool, NULL, NULL);

	LPDIRECT3DINDEXBUFFER9 ib9 = NULL;
	HRESULT ret = d3d9dev->CreateIndexBuffer(Length, Usage, ConvertD3DFORMAT_8to9(Format), Pool, &ib9, NULL);

	IDirect3DIndexBuffer8Hook* ibhook = ObjLookupCreate(ib9, this);
	*ppIndexBuffer = ibhook;

	return ret;
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DDevice8Hook::CreateRenderTarget(THIS_ UINT Width, UINT Height, d3d8::D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, BOOL Lockable, IDirect3DSurface8** ppSurface)
{
	if (!ppSurface)
		return d3d9dev->CreateRenderTarget(Width, Height, ConvertD3DFORMAT_8to9(Format), MultiSample, 0, Lockable, NULL, NULL);

	LPDIRECT3DSURFACE9 surface9 = NULL;
	HRESULT ret = d3d9dev->CreateRenderTarget(Width, Height, ConvertD3DFORMAT_8to9(Format), MultiSample, 0, Lockable, &surface9, NULL);

	IDirect3DSurface8Hook* surfacehook = ObjLookupCreate(surface9, this);
	*ppSurface = surfacehook;

	return ret;
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DDevice8Hook::CreateDepthStencilSurface(THIS_ UINT Width, UINT Height, d3d8::D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, IDirect3DSurface8** ppSurface)
{
	static const bool Discard = TRUE;
	
	if (!ppSurface)
		return d3d9dev->CreateDepthStencilSurface(Width, Height, ConvertD3DFORMAT_8to9(Format), MultiSample, 0, Discard, NULL, NULL);

	LPDIRECT3DSURFACE9 surface9 = NULL;
	HRESULT ret = d3d9dev->CreateDepthStencilSurface(Width, Height, ConvertD3DFORMAT_8to9(Format), MultiSample, 0, Discard, &surface9, NULL);

	IDirect3DSurface8Hook* surfacehook = ObjLookupCreate(surface9, this);
	*ppSurface = surfacehook;

	return ret;
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DDevice8Hook::UpdateTexture(THIS_ IDirect3DBaseTexture8* pSourceTexture, IDirect3DBaseTexture8* pDestinationTexture)
{
	if (!pSourceTexture && !pDestinationTexture)
		return d3d9dev->UpdateTexture(NULL, NULL);

	{
		IDirect3DCubeTexture8Hook* cubeTextureSource = dynamic_cast<IDirect3DCubeTexture8Hook*>(pSourceTexture);
		IDirect3DCubeTexture8Hook* cubeTextureDest = dynamic_cast<IDirect3DCubeTexture8Hook*>(pDestinationTexture);
		if (cubeTextureSource || cubeTextureDest)
			return d3d9dev->UpdateTexture(cubeTextureSource ? cubeTextureSource->GetUnderlyingCubeTexture() : NULL, cubeTextureDest ? cubeTextureDest->GetUnderlyingCubeTexture() : NULL);
	}

	{
		IDirect3DVolumeTexture8Hook* volumeTextureSource = dynamic_cast<IDirect3DVolumeTexture8Hook*>(pSourceTexture);
		IDirect3DVolumeTexture8Hook* volumeTextureDest = dynamic_cast<IDirect3DVolumeTexture8Hook*>(pDestinationTexture);
		if (volumeTextureSource || volumeTextureDest)
			return d3d9dev->UpdateTexture(volumeTextureSource ? volumeTextureSource->GetUnderlyingVolumeTexture() : NULL, volumeTextureDest ? volumeTextureDest->GetUnderlyingVolumeTexture() : NULL);
	}

	{
		IDirect3DTexture8Hook* TextureSource = dynamic_cast<IDirect3DTexture8Hook*>(pSourceTexture);
		IDirect3DTexture8Hook* TextureDest = dynamic_cast<IDirect3DTexture8Hook*>(pDestinationTexture);
		if (TextureSource || TextureDest)
			return d3d9dev->UpdateTexture(TextureSource ? TextureSource->GetUnderlyingTexture() : NULL, TextureDest ? TextureDest->GetUnderlyingTexture() : NULL);
	}

	{
		IDirect3DBaseTexture8Hook* baseTextureSource = dynamic_cast<IDirect3DBaseTexture8Hook*>(pSourceTexture);
		IDirect3DBaseTexture8Hook* baseTextureDest = dynamic_cast<IDirect3DBaseTexture8Hook*>(pDestinationTexture);
		if (baseTextureSource || baseTextureDest)
			return d3d9dev->UpdateTexture(baseTextureSource ? baseTextureSource->GetUnderlyingBaseTexture() : NULL, baseTextureDest ? baseTextureDest->GetUnderlyingBaseTexture() : NULL);
	}

	return d3d9dev->UpdateTexture(NULL, NULL);
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DDevice8Hook::SetRenderTarget(THIS_ IDirect3DSurface8* pRenderTarget,IDirect3DSurface8* pNewZStencil)
{
	IDirect3DSurface8Hook* renderTargetHook = dynamic_cast<IDirect3DSurface8Hook*>(pRenderTarget);
	IDirect3DSurface8Hook* zStencilHook = dynamic_cast<IDirect3DSurface8Hook*>(pNewZStencil);
	HRESULT setRTHR = d3d9dev->SetRenderTarget(0, renderTargetHook ? renderTargetHook->GetUnderlyingSurface() : NULL);
	HRESULT setZStencilHR = d3d9dev->SetDepthStencilSurface(zStencilHook ? zStencilHook->GetUnderlyingSurface() : NULL);
	if (FAILED(setRTHR) )
		return setRTHR;
	else if (FAILED(setZStencilHR) )
		return setZStencilHR;
	else
		// Success code
		return setRTHR;
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DDevice8Hook::GetRenderTarget(THIS_ IDirect3DSurface8** ppRenderTarget)
{
	if (!ppRenderTarget)
		return d3d9dev->GetRenderTarget(0, NULL);

	LPDIRECT3DSURFACE9 surface9 = NULL;
	HRESULT ret = d3d9dev->GetRenderTarget(0, &surface9);

	IDirect3DSurface8Hook* surfacehook = ObjLookupCreate(surface9, this);
	*ppRenderTarget = surfacehook;

	return ret;
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DDevice8Hook::GetDepthStencilSurface(THIS_ IDirect3DSurface8** ppZStencilSurface)
{
	if (!ppZStencilSurface)
		return d3d9dev->GetDepthStencilSurface(NULL);

	LPDIRECT3DSURFACE9 surface9 = NULL;
	HRESULT ret = d3d9dev->GetDepthStencilSurface(&surface9);

	IDirect3DSurface8Hook* surfacehook = ObjLookupCreate(surface9, this);
	*ppZStencilSurface = surfacehook;

	return ret;
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DDevice8Hook::BeginScene(THIS)
{
	return d3d9dev->BeginScene();
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DDevice8Hook::EndScene(THIS)
{
	return d3d9dev->EndScene();
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DDevice8Hook::Clear(THIS_ DWORD Count, CONST D3DRECT* pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil)
{
	return d3d9dev->Clear(Count, pRects, Flags, Color, Z, Stencil);
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DDevice8Hook::SetTransform(THIS_ D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX* pMatrix)
{
	return d3d9dev->SetTransform(State, pMatrix);
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DDevice8Hook::GetTransform(THIS_ D3DTRANSFORMSTATETYPE State, D3DMATRIX* pMatrix)
{
	return d3d9dev->GetTransform(State, pMatrix);
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DDevice8Hook::MultiplyTransform(THIS_ D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX* pMatrix)
{
	return d3d9dev->MultiplyTransform(State, pMatrix);
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DDevice8Hook::SetViewport(THIS_ CONST D3DVIEWPORT8* pViewport)
{
	// This is an okay cast because D3DVIEWPORT8 and D3DVIEWPORT9 are identical structs
	return d3d9dev->SetViewport( (D3DVIEWPORT9*)pViewport);
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DDevice8Hook::GetViewport(THIS_ D3DVIEWPORT8* pViewport)
{
	// This is an okay cast because D3DVIEWPORT8 and D3DVIEWPORT9 are identical structs
	return d3d9dev->GetViewport( (D3DVIEWPORT9*)pViewport);
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DDevice8Hook::SetMaterial(THIS_ CONST D3DMATERIAL8* pMaterial)
{
	// This is an okay cast because D3DMATERIAL8 and D3DMATERIAL9 are identical structs
	return d3d9dev->SetMaterial( (CONST D3DMATERIAL9*)pMaterial);
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DDevice8Hook::GetMaterial(THIS_ D3DMATERIAL8* pMaterial)
{
	// This is an okay cast because D3DMATERIAL8 and D3DMATERIAL9 are identical structs
	return d3d9dev->GetMaterial( (D3DMATERIAL9*)pMaterial);
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DDevice8Hook::SetLight(THIS_ DWORD Index, CONST D3DLIGHT8* Light)
{
	// This is an okay cast becase D3DLIGHT8 and D3DLIGHT9 are identical structs
	return d3d9dev->SetLight(Index, (CONST D3DLIGHT9*)Light);
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DDevice8Hook::GetLight(THIS_ DWORD Index, D3DLIGHT8* Light)
{
	// This is an okay cast becase D3DLIGHT8 and D3DLIGHT9 are identical structs
	return d3d9dev->GetLight(Index, (D3DLIGHT9*)Light);
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DDevice8Hook::LightEnable(THIS_ DWORD Index, BOOL Enable)
{
	return d3d9dev->LightEnable(Index, Enable);
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DDevice8Hook::GetLightEnable(THIS_ DWORD Index, BOOL* pEnable)
{
	return d3d9dev->GetLightEnable(Index, pEnable);
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DDevice8Hook::SetClipPlane(THIS_ DWORD Index, CONST float* pPlane)
{
	return d3d9dev->SetClipPlane(Index, pPlane);
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DDevice8Hook::GetClipPlane(THIS_ DWORD Index, float* pPlane)
{
	return d3d9dev->GetClipPlane(Index, pPlane);
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DDevice8Hook::SetRenderState(THIS_ d3d8::D3DRENDERSTATETYPE State, DWORD Value)
{
	switch (State)
	{
	case d3d8::D3DRS_ZVISIBLE:
	case d3d8::D3DRS_LINEPATTERN:
	case d3d8::D3DRS_EDGEANTIALIAS:
		return S_OK; // No longer supported in d3d9, do nothing!
	case d3d8::D3DRS_PATCHSEGMENTS:
		return d3d9dev->SetNPatchMode(*(const float* const)&Value);
	case d3d8::D3DRS_SOFTWAREVERTEXPROCESSING:
		return d3d9dev->SetSoftwareVertexProcessing(Value); // TODO: Record this in state blocks
	case d3d8::D3DRS_ZBIAS:
		return d3d9dev->SetRenderState(D3DRS_DEPTHBIAS, Value); // TODO: Convert from D3D8-style Depth Bias (integer) to D3D9-style Depth Bias (float)
	default:
		return d3d9dev->SetRenderState( (const D3DRENDERSTATETYPE)State, Value);
	}
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DDevice8Hook::GetRenderState(THIS_ d3d8::D3DRENDERSTATETYPE State, DWORD* pValue)
{
	switch (State)
	{
	case d3d8::D3DRS_ZVISIBLE:
	case d3d8::D3DRS_LINEPATTERN:
	case d3d8::D3DRS_EDGEANTIALIAS:
		if (pValue)
			*pValue = 0;
		return S_OK; // No longer supported in d3d9, do nothing!
	case d3d8::D3DRS_PATCHSEGMENTS:
		if (pValue)
			*(float* const)pValue = d3d9dev->GetNPatchMode();
		return S_OK;
	case d3d8::D3DRS_SOFTWAREVERTEXPROCESSING:
		if (pValue)
			*pValue = d3d9dev->GetSoftwareVertexProcessing(); // TODO: Record this in state blocks
		return S_OK;
	case d3d8::D3DRS_ZBIAS:
		return d3d9dev->GetRenderState(D3DRS_DEPTHBIAS, pValue); // TODO: Convert from D3D8-style Depth Bias (integer) to D3D9-style Depth Bias (float)
	default:
		return d3d9dev->GetRenderState( (const D3DRENDERSTATETYPE)State, pValue);
	}
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DDevice8Hook::CreateStateBlock(THIS_ D3DSTATEBLOCKTYPE Type, DWORD* pToken)
{
	if (!pToken)
		return d3d9dev->CreateStateBlock(Type, NULL);

	LPDIRECT3DSTATEBLOCK9 sb9 = NULL;
	HRESULT ret = d3d9dev->CreateStateBlock(Type, &sb9);

	IDirect3DStateBlock8Hook* sbhook = ObjLookupCreate(sb9, this);
	sbhook->StateBlockType = Type;
	if (Type & D3DSBT_VERTEXSTATE)
		sbhook->BaseVertexIndex = BaseVertexIndex;
	*pToken = (const DWORD)sbhook;

	return ret;
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DDevice8Hook::BeginStateBlock(THIS)
{
	return d3d9dev->BeginStateBlock();
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DDevice8Hook::EndStateBlock(THIS_ DWORD* pToken)
{
	if (!pToken)
		return d3d9dev->EndStateBlock(NULL);

	LPDIRECT3DSTATEBLOCK9 sb9 = NULL;
	HRESULT ret = d3d9dev->EndStateBlock(&sb9);

	IDirect3DStateBlock8Hook* sbhook = ObjLookupCreate(sb9, this);
	sbhook->StateBlockType = D3DSBT_ALL; // TODO: Is this correct?
	sbhook->BaseVertexIndex = BaseVertexIndex;
	*pToken = (const DWORD)sbhook;

	return ret;
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DDevice8Hook::SetClipStatus(THIS_ CONST D3DCLIPSTATUS8* pClipStatus)
{
	return d3d9dev->SetClipStatus( (const D3DCLIPSTATUS9* const)pClipStatus);
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DDevice8Hook::GetClipStatus(THIS_ D3DCLIPSTATUS8* pClipStatus)
{
	return d3d9dev->GetClipStatus( (D3DCLIPSTATUS9* const)pClipStatus);
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DDevice8Hook::GetTexture(THIS_ DWORD Stage, IDirect3DBaseTexture8** ppTexture)
{
	if (!ppTexture)
		return d3d9dev->GetTexture(Stage, NULL);

	LPDIRECT3DBASETEXTURE9 basetexture9 = NULL;
	HRESULT ret = d3d9dev->GetTexture(Stage, &basetexture9);

	if (basetexture9)
	{
		LPDIRECT3DCUBETEXTURE9 cubetex9 = NULL;
		LPDIRECT3DVOLUMETEXTURE9 volumetex9 = NULL;
		LPDIRECT3DTEXTURE9 tex9 = NULL;
		LPDIRECT3DBASETEXTURE9 basetex9 = NULL;
		if (basetexture9->QueryInterface(IID_IDirect3DCubeTexture9, (void**)&cubetex9) )
		{
			IDirect3DCubeTexture8Hook* cubetexturehook = ObjLookupCreate(cubetex9, this);
			*ppTexture = cubetexturehook;
		}
		else if (basetexture9->QueryInterface(IID_IDirect3DVolumeTexture9, (void**)&volumetex9) )
		{
			IDirect3DVolumeTexture8Hook* volumetexturehook = ObjLookupCreate(volumetex9, this);
			*ppTexture = volumetexturehook;
		}
		else if (basetexture9->QueryInterface(IID_IDirect3DTexture9, (void**)&tex9) )
		{
			IDirect3DTexture8Hook* texturehook = ObjLookupCreate(tex9, this);
			*ppTexture = texturehook;
		}
		else if (basetexture9->QueryInterface(IID_IDirect3DBaseTexture9, (void**)&basetex9) )
		{
			IDirect3DBaseTexture8Hook* basetexturehook = ObjLookupCreate(basetex9, this);
			*ppTexture = basetexturehook;
		}
	}

	return ret;
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DDevice8Hook::SetTexture(THIS_ DWORD Stage, IDirect3DBaseTexture8* pTexture)
{
	if (!pTexture)
		return d3d9dev->SetTexture(Stage, NULL);

	IDirect3DCubeTexture8Hook* cubehook = dynamic_cast<IDirect3DCubeTexture8Hook*>(pTexture);
	if (cubehook)
		return d3d9dev->SetTexture(Stage, cubehook->GetUnderlyingCubeTexture() );

	IDirect3DVolumeTexture8Hook* volumehook = dynamic_cast<IDirect3DVolumeTexture8Hook*>(pTexture);
	if (volumehook)
		return d3d9dev->SetTexture(Stage, volumehook->GetUnderlyingVolumeTexture() );

	IDirect3DTexture8Hook* texhook = dynamic_cast<IDirect3DTexture8Hook*>(pTexture);
	if (texhook)
		return d3d9dev->SetTexture(Stage, texhook->GetUnderlyingTexture() );

	IDirect3DBaseTexture8Hook* basetexhook = dynamic_cast<IDirect3DBaseTexture8Hook*>(pTexture);
	if (basetexhook)
		return d3d9dev->SetTexture(Stage, basetexhook->GetUnderlyingBaseTexture() );

	return d3d9dev->SetTexture(Stage, NULL);
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DDevice8Hook::GetTextureStageState(THIS_ DWORD Stage, d3d8::D3DTEXTURESTAGESTATETYPE Type, DWORD* pValue)
{
	// Break this into SetTextureStageState + SetSamplerState as per https://msdn.microsoft.com/en-us/library/windows/desktop/bb204851(v=vs.85).aspx#Texture_Sampler_Changes
	switch (Type)
	{
	case D3DTSS_COLOROP:
	case D3DTSS_COLORARG1:
	case D3DTSS_COLORARG2:
	case D3DTSS_ALPHAOP:
	case D3DTSS_ALPHAARG1:
	case D3DTSS_ALPHAARG2:
	case D3DTSS_BUMPENVMAT00:
	case D3DTSS_BUMPENVMAT01:
	case D3DTSS_BUMPENVMAT10:
	case D3DTSS_BUMPENVMAT11:
	case D3DTSS_TEXCOORDINDEX:
	case D3DTSS_BUMPENVLSCALE:
	case D3DTSS_BUMPENVLOFFSET:
	case D3DTSS_TEXTURETRANSFORMFLAGS:
	case D3DTSS_COLORARG0:
	case D3DTSS_ALPHAARG0:
	case D3DTSS_RESULTARG:
	case D3DTSS_CONSTANT: // This is a d3d9-only type
	default:
		return d3d9dev->GetTextureStageState(Stage, (const D3DTEXTURESTAGESTATETYPE)Type, pValue);
	case d3d8::D3DTSS_ADDRESSU:
		return d3d9dev->GetSamplerState(Stage, D3DSAMP_ADDRESSU, pValue);
	case d3d8::D3DTSS_ADDRESSV:
		return d3d9dev->GetSamplerState(Stage, D3DSAMP_ADDRESSV, pValue);
	case d3d8::D3DTSS_ADDRESSW:
		return d3d9dev->GetSamplerState(Stage, D3DSAMP_ADDRESSW, pValue);
	case d3d8::D3DTSS_BORDERCOLOR:
		return d3d9dev->GetSamplerState(Stage, D3DSAMP_BORDERCOLOR, pValue);
	case d3d8::D3DTSS_MAGFILTER:
		return d3d9dev->GetSamplerState(Stage, D3DSAMP_MAGFILTER, pValue);
	case d3d8::D3DTSS_MINFILTER:
		return d3d9dev->GetSamplerState(Stage, D3DSAMP_MINFILTER, pValue);
	case d3d8::D3DTSS_MIPFILTER:
		return d3d9dev->GetSamplerState(Stage, D3DSAMP_MIPFILTER, pValue);
	case d3d8::D3DTSS_MIPMAPLODBIAS:
		return d3d9dev->GetSamplerState(Stage, D3DSAMP_MIPMAPLODBIAS, pValue);
	case d3d8::D3DTSS_MAXMIPLEVEL:
		return d3d9dev->GetSamplerState(Stage, D3DSAMP_MAXMIPLEVEL, pValue);
	case d3d8::D3DTSS_MAXANISOTROPY:
		return d3d9dev->GetSamplerState(Stage, D3DSAMP_MAXANISOTROPY, pValue);
	}
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DDevice8Hook::SetTextureStageState(THIS_ DWORD Stage, d3d8::D3DTEXTURESTAGESTATETYPE Type, DWORD Value)
{
	// Break this into SetTextureStageState + SetSamplerState as per https://msdn.microsoft.com/en-us/library/windows/desktop/bb204851(v=vs.85).aspx#Texture_Sampler_Changes
	switch (Type)
	{
	case D3DTSS_COLOROP:
	case D3DTSS_COLORARG1:
	case D3DTSS_COLORARG2:
	case D3DTSS_ALPHAOP:
	case D3DTSS_ALPHAARG1:
	case D3DTSS_ALPHAARG2:
	case D3DTSS_BUMPENVMAT00:
	case D3DTSS_BUMPENVMAT01:
	case D3DTSS_BUMPENVMAT10:
	case D3DTSS_BUMPENVMAT11:
	case D3DTSS_TEXCOORDINDEX:
	case D3DTSS_BUMPENVLSCALE:
	case D3DTSS_BUMPENVLOFFSET:
	case D3DTSS_TEXTURETRANSFORMFLAGS:
	case D3DTSS_COLORARG0:
	case D3DTSS_ALPHAARG0:
	case D3DTSS_RESULTARG:
	case D3DTSS_CONSTANT: // This is a d3d9-only type
	default:
		return d3d9dev->SetTextureStageState(Stage, (const D3DTEXTURESTAGESTATETYPE)Type, Value);
	case d3d8::D3DTSS_ADDRESSU:
		return d3d9dev->SetSamplerState(Stage, D3DSAMP_ADDRESSU, Value);
	case d3d8::D3DTSS_ADDRESSV:
		return d3d9dev->SetSamplerState(Stage, D3DSAMP_ADDRESSV, Value);
	case d3d8::D3DTSS_ADDRESSW:
		return d3d9dev->SetSamplerState(Stage, D3DSAMP_ADDRESSW, Value);
	case d3d8::D3DTSS_BORDERCOLOR:
		return d3d9dev->SetSamplerState(Stage, D3DSAMP_BORDERCOLOR, Value);
	case d3d8::D3DTSS_MAGFILTER:
		return d3d9dev->SetSamplerState(Stage, D3DSAMP_MAGFILTER, Value);
	case d3d8::D3DTSS_MINFILTER:
		return d3d9dev->SetSamplerState(Stage, D3DSAMP_MINFILTER, Value);
	case d3d8::D3DTSS_MIPFILTER:
		return d3d9dev->SetSamplerState(Stage, D3DSAMP_MIPFILTER, Value);
	case d3d8::D3DTSS_MIPMAPLODBIAS:
		return d3d9dev->SetSamplerState(Stage, D3DSAMP_MIPMAPLODBIAS, Value);
	case d3d8::D3DTSS_MAXMIPLEVEL:
		return d3d9dev->SetSamplerState(Stage, D3DSAMP_MAXMIPLEVEL, Value);
	case d3d8::D3DTSS_MAXANISOTROPY:
		return d3d9dev->SetSamplerState(Stage, D3DSAMP_MAXANISOTROPY, Value);
	}
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DDevice8Hook::ValidateDevice(THIS_ DWORD* pNumPasses)
{
	return d3d9dev->ValidateDevice(pNumPasses);
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DDevice8Hook::SetPaletteEntries(THIS_ UINT PaletteNumber, CONST PALETTEENTRY* pEntries)
{
	return d3d9dev->SetPaletteEntries(PaletteNumber, pEntries);
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DDevice8Hook::GetPaletteEntries(THIS_ UINT PaletteNumber, PALETTEENTRY* pEntries)
{
	return d3d9dev->GetPaletteEntries(PaletteNumber, pEntries);
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DDevice8Hook::SetCurrentTexturePalette(THIS_ UINT PaletteNumber)
{
	return d3d9dev->SetCurrentTexturePalette(PaletteNumber);
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DDevice8Hook::GetCurrentTexturePalette(THIS_ UINT *PaletteNumber)
{
	return d3d9dev->GetCurrentTexturePalette(PaletteNumber);
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DDevice8Hook::DrawPrimitive(THIS_ D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount)
{
	return d3d9dev->DrawPrimitive(PrimitiveType, StartVertex, PrimitiveCount);
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DDevice8Hook::DrawIndexedPrimitive(THIS_ D3DPRIMITIVETYPE PrimitiveType, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount)
{
	return d3d9dev->DrawIndexedPrimitive(PrimitiveType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DDevice8Hook::DrawPrimitiveUP(THIS_ D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride)
{
	return d3d9dev->DrawPrimitiveUP(PrimitiveType, PrimitiveCount, pVertexStreamZeroData, VertexStreamZeroStride);
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DDevice8Hook::DrawIndexedPrimitiveUP(THIS_ D3DPRIMITIVETYPE PrimitiveType, UINT MinVertexIndex, UINT NumVertices, UINT PrimitiveCount, CONST void* pIndexData, D3DFORMAT IndexDataFormat, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride)
{
	return d3d9dev->DrawIndexedPrimitiveUP(PrimitiveType, MinVertexIndex, NumVertices, PrimitiveCount, pIndexData, IndexDataFormat, pVertexStreamZeroData, VertexStreamZeroStride);
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DDevice8Hook::ProcessVertices(THIS_ UINT SrcStartIndex, UINT DestIndex, UINT VertexCount, IDirect3DVertexBuffer8* pDestBuffer, DWORD Flags)
{
	if (!pDestBuffer)
		return d3d9dev->ProcessVertices(SrcStartIndex, DestIndex, VertexCount, NULL, NULL, Flags);

	IDirect3DVertexBuffer8Hook* vbhook = dynamic_cast<IDirect3DVertexBuffer8Hook*>(pDestBuffer);

	return d3d9dev->ProcessVertices(SrcStartIndex, DestIndex, VertexCount, vbhook ? vbhook->GetUnderlyingVertexBuffer() : NULL, NULL, Flags);
}

static inline const unsigned GetDecltypeSizeBytes(const D3DDECLTYPE type)
{
	switch (type)
	{
	case D3DDECLTYPE_FLOAT1:  // 1D float expanded to (value, 0., 0., 1.)
		return sizeof(float);
	case D3DDECLTYPE_FLOAT2:  // 2D float expanded to (value, value, 0., 1.)
		return sizeof(float) * 2;
	case D3DDECLTYPE_FLOAT3:  // 3D float expanded to (value, value, value, 1.)
		return sizeof(float) * 3;
	case D3DDECLTYPE_FLOAT4:  // 4D float
		return sizeof(float) * 4;
	case D3DDECLTYPE_D3DCOLOR:  // 4D packed unsigned bytes mapped to 0. to 1. range
                                 // Input is in D3DCOLOR format (ARGB) expanded to (R, G, B, A)
		return sizeof(D3DCOLOR);
	case D3DDECLTYPE_UBYTE4:  // 4D unsigned byte
		return sizeof(BYTE) * 4;
	case D3DDECLTYPE_SHORT2:  // 2D signed short expanded to (value, value, 0., 1.)
		return sizeof(SHORT) * 2;
	case D3DDECLTYPE_SHORT4:  // 4D signed short
		return sizeof(SHORT) * 4;

// The following types are valid only with vertex shaders >= 2.0

	case D3DDECLTYPE_UBYTE4N:  // Each of 4 bytes is normalized by dividing to 255.0
		return sizeof(BYTE) * 4;
	case D3DDECLTYPE_SHORT2N:  // 2D signed short normalized (v[0]/32767.0,v[1]/32767.0,0,1)
		return sizeof(SHORT) * 2;
	case D3DDECLTYPE_SHORT4N:  // 4D signed short normalized (v[0]/32767.0,v[1]/32767.0,v[2]/32767.0,v[3]/32767.0)
		return sizeof(SHORT) * 4;
	case D3DDECLTYPE_USHORT2N:  // 2D unsigned short normalized (v[0]/65535.0,v[1]/65535.0,0,1)
		return sizeof(USHORT) * 2;
	case D3DDECLTYPE_USHORT4N:  // 4D unsigned short normalized (v[0]/65535.0,v[1]/65535.0,v[2]/65535.0,v[3]/65535.0)
		return sizeof(USHORT) * 4;
    case D3DDECLTYPE_UDEC3:  // 3D unsigned 10 10 10 format expanded to (value, value, value, 1)
		return 4;
    case D3DDECLTYPE_DEC3N:  // 3D signed 10 10 10 format normalized and expanded to (v[0]/511.0, v[1]/511.0, v[2]/511.0, 1)
		return 4;
    case D3DDECLTYPE_FLOAT16_2:  // Two 16-bit floating point values, expanded to (value, value, 0, 1)
		return sizeof(unsigned short) * 2;
    case D3DDECLTYPE_FLOAT16_4:  // Four 16-bit floating point values
		return sizeof(unsigned short) * 4;
	default:
	case D3DDECLTYPE_UNUSED:
		return 0;
	}
}

struct UsageAndIndex
{
	BYTE usage;
	BYTE index;
};
static_assert(sizeof(UsageAndIndex) == 2, "Error");

struct constFloatReg
{
	D3DCOLORVALUE initialValue;
	BYTE constRegIndex;
};

struct vertDeclRegisterMapping
{
	WORD inputRegisterIndex; // Which input register in the shader this maps to
	BYTE usageType; // The usage type and index of the declaration
	BYTE usageIndex;
};

static inline void ConvertVertexDecl8to9(const DWORD* inDecl8, std::vector<D3DVERTEXELEMENT9>& outDecl9, std::vector<constFloatReg>& constFloatRegs, std::vector<vertDeclRegisterMapping>& declRegisterMapping)
{
	// This is the current stream for our vertex declaration
	WORD streamID = 0;

	// Determines if the current stream is for tessellation or non-tessellation (default) vertex data
	bool isTessellatorStream = false;
	
	// This is the per-stream buffer offset (in bytes)
	WORD streamOffsets[16] = {0};

	static BYTE usageCounts[MAXD3DDECLUSAGE + 1] = {0};

	static const D3DDECLUSAGE usageMap8to9[D3DVSDE_NORMAL2 + 1] =
	{
		D3DDECLUSAGE_POSITION, // Position0
		D3DDECLUSAGE_BLENDWEIGHT, // BlendWeight 0
		D3DDECLUSAGE_BLENDINDICES, // BlendIndices 0
		D3DDECLUSAGE_NORMAL, // Normal0
		D3DDECLUSAGE_PSIZE, // PSize0
		D3DDECLUSAGE_COLOR, // Color0
		D3DDECLUSAGE_COLOR, // Color1
		D3DDECLUSAGE_TEXCOORD, // Texcoord0
		D3DDECLUSAGE_TEXCOORD, // Texcoord1
		D3DDECLUSAGE_TEXCOORD, // Texcoord2
		D3DDECLUSAGE_TEXCOORD, // Texcoord3
		D3DDECLUSAGE_TEXCOORD, // Texcoord4
		D3DDECLUSAGE_TEXCOORD, // Texcoord5
		D3DDECLUSAGE_TEXCOORD, // Texcoord6
		D3DDECLUSAGE_TEXCOORD, // Texcoord7
		D3DDECLUSAGE_POSITION, // Position1
		D3DDECLUSAGE_NORMAL // Normal1
	};

	static const BYTE usageIndexMap8to9[D3DVSDE_NORMAL2 + 1] =
	{
		0, // Position0
		0, // BlendWeight0
		0, // BlendIndices0
		0, // Normal0
		0, // PSize0
		0, // Color0
		1, // Color1
		0, // Texcoord0
		1, // Texcoord1
		2, // Texcoord2
		3, // Texcoord3
		4, // Texcoord4
		5, // Texcoord5
		6, // Texcoord6
		7, // Texcoord7
		1, // Position1
		1  // Normal1
	};

	while (*inDecl8 != D3DVSD_END() )
	{
		const D3DVSD_TOKENTYPE type = (const D3DVSD_TOKENTYPE)( (*inDecl8 & D3DVSD_TOKENTYPEMASK) >> D3DVSD_TOKENTYPESHIFT);

		switch (type)
		{
		default:
		case D3DVSD_TOKEN_NOP:
			++inDecl8;
			break;
		case D3DVSD_TOKEN_STREAM:
			streamID = (*inDecl8 & D3DVSD_STREAMNUMBERMASK) >> D3DVSD_STREAMNUMBERSHIFT;
			isTessellatorStream = *inDecl8 & (1 << 28) ? true : false;
			++inDecl8;
			break;
		case D3DVSD_TOKEN_TESSELLATOR:
		case D3DVSD_TOKEN_STREAMDATA:
		{
			// Skip register:
			if (type == D3DVSD_TOKEN_STREAMDATA && *inDecl8 & 0x10000000)
			{
				streamOffsets[streamID] += ( (*inDecl8 & D3DVSD_SKIPCOUNTMASK) >> D3DVSD_SKIPCOUNTSHIFT);
			}
			// Load register
			else
			{
				// The D3D9 D3DDECLTYPE enum was designed to replace the D3D8-style hardcoded type numbers:
				const D3DDECLTYPE dataType = (const D3DDECLTYPE)( (*inDecl8 & D3DVSD_DATATYPEMASK) >> D3DVSD_DATATYPESHIFT);
				const DWORD registerIndex = (*inDecl8 & 0x1F);

				D3DVERTEXELEMENT9 newVertexElement = {0};
				newVertexElement.Stream = streamID;
				newVertexElement.Offset = streamOffsets[streamID];
				newVertexElement.Type = dataType;
				newVertexElement.Method = isTessellatorStream ? D3DDECLMETHOD_CROSSUV : D3DDECLMETHOD_DEFAULT;
				newVertexElement.Usage = usageMap8to9[registerIndex];
				newVertexElement.UsageIndex = usageIndexMap8to9[registerIndex];

				outDecl9.push_back(newVertexElement);

				// Generate a new entry in the vertex-decl to shader input register mapping:
				vertDeclRegisterMapping newMapping;
				newMapping.inputRegisterIndex = registerIndex & 0xF;
				newMapping.usageType = newVertexElement.Usage;
				newMapping.usageIndex = newVertexElement.UsageIndex;
				declRegisterMapping.push_back(newMapping);

				// Advance the stream offset:
				streamOffsets[streamID] += GetDecltypeSizeBytes(dataType);
			}

			// Advance past this current token
			++inDecl8;
		}
			break;
		case D3DVSD_TOKEN_CONSTMEM:
		{
			// Since these are shader-register-sized increments, we're really advancing by 4 * sizeof(DWORD) or sizeof(float4):
			const DWORD numConstRegisters = ( (*inDecl8 & D3DVSD_CONSTCOUNTMASK) >> D3DVSD_CONSTCOUNTSHIFT);
			const DWORD numDWORDsConsts = 4 * numConstRegisters;

			const DWORD registerBaseAddress = *inDecl8 & D3DVSD_CONSTADDRESSMASK;
			const D3DCOLORVALUE* const registersData = (const D3DCOLORVALUE* const)(inDecl8 + 1);
			for (unsigned x = 0; x < numConstRegisters; ++x)
			{
				constFloatReg newFloatReg;
				newFloatReg.constRegIndex = (const BYTE)(registerBaseAddress + x);
				newFloatReg.initialValue = registersData[x];
				constFloatRegs.push_back(newFloatReg);
			}

			inDecl8 += numDWORDsConsts + 1;
		}
			break;
		case D3DVSD_TOKEN_EXT:
		{
			const DWORD numDWORDsExtension = (*inDecl8 & D3DVSD_EXTCOUNTMASK) >> D3DVSD_EXTCOUNTSHIFT;
			inDecl8 += numDWORDsExtension + 1;
		}
			break;
		case D3DVSD_TOKEN_END:
			goto endLoop;
			break;
		}
	}

endLoop:

	static const D3DVERTEXELEMENT9 endElement = D3DDECL_END();
	outDecl9.push_back(endElement);
}

static inline void CopyModifyVertShaderBytecode(DWORD* pFunction, std::vector<DWORD>& outputBytecode, const ShaderInfo& originalShaderInfo, const std::vector<constFloatReg>& constFloatRegs, const std::vector<vertDeclRegisterMapping>& declRegisterMapping)
{
	versionToken version = *(const versionToken* const)pFunction++;

	if (version.pixelOrVertexShader != 0xFFFE)
	{
#ifdef _DEBUG
		// Not a vertex shader!
		__debugbreak();
#endif
		return;
	}

	if (version.majorVersion != 1)
	{
#ifdef _DEBUG
		// Not vs_1_1 or vs_1_0 shader!
		__debugbreak();
#endif
		return;
	}

	if (version.minorVersion < 1)
	{
		// Fixup the version from vs_1_0 to vs_1_1 because D3D9 no longer
		// accepts vs_1_0 shaders
		version.minorVersion = 1;
	}

	// Fix up writing to all channels of output registers by modifying their dstParameter write masks.
	// This is necessary because in D3D8 vs_1_0 and D3D8 vs_1_1 it was legal to leave some of the output registers' channels unwritten.
	// However, D3D9 shaders will fail validation if not all of the components are written to (until D3D9 SM3.0 when they re-added this feature).
	const unsigned numWrittenOutputRegs = originalShaderInfo.writtenOutputRegisters.size();
	for (unsigned x = 0; x < numWrittenOutputRegs; ++x)
	{
		const WrittenOutputRegister& writtenOutputReg = originalShaderInfo.writtenOutputRegisters[x];

		if (writtenOutputReg.registerType == D3DSPR_RASTOUT && writtenOutputReg.componentChannelsWritten < 0xF)
		{
			const unsigned numWriteOperations = writtenOutputReg.writeOperationsHistory.size();
			const outputRegisterWriteTracker::outputRegisterWriteOperation& writeOperation = writtenOutputReg.writeOperationsHistory[numWriteOperations - 1];

			// Change the dstParameter's write mask from whatever it was (less than 0xF) to write all channels (0xF) by writing in the missing channels
			switch (writtenOutputReg.componentChannelsWritten)
			{
			case 0x0: // Written nothing, needs to write everything!
				*(DWORD* const)writeOperation.registerParameterToken |= D3DSP_WRITEMASK_ALL;
				break;
			case 0x1: // Just .x is written, we need to add .yzw
				*(DWORD* const)writeOperation.registerParameterToken |= (D3DSP_WRITEMASK_1 | D3DSP_WRITEMASK_2 | D3DSP_WRITEMASK_3);
				break;
			case 0x2: // Just .y is written, we need to add .xzw
				*(DWORD* const)writeOperation.registerParameterToken |= (D3DSP_WRITEMASK_0 | D3DSP_WRITEMASK_2 | D3DSP_WRITEMASK_3);
				break;
			case 0x3: // Just .xy is written, we need to add .zw
				*(DWORD* const)writeOperation.registerParameterToken |= (D3DSP_WRITEMASK_2 | D3DSP_WRITEMASK_3);
				break;
			case 0x4: // Just .z is written, we need to add .xyw
				*(DWORD* const)writeOperation.registerParameterToken |= (D3DSP_WRITEMASK_0 | D3DSP_WRITEMASK_1 | D3DSP_WRITEMASK_3);
				break;
			case 0x5: // Just .xz is written, we need to add .yw
				*(DWORD* const)writeOperation.registerParameterToken |= (D3DSP_WRITEMASK_1 | D3DSP_WRITEMASK_3);
				break;
			case 0x6: // Just .yz is written, we need to add .xw
				*(DWORD* const)writeOperation.registerParameterToken |= (D3DSP_WRITEMASK_0 | D3DSP_WRITEMASK_3);
				break;
			case 0x7: // .xyz is written, only .w is missing
				*(DWORD* const)writeOperation.registerParameterToken |= (D3DSP_WRITEMASK_3);
				break;
			case 0x8: // Just .w is written, we need to add .xyz
				*(DWORD* const)writeOperation.registerParameterToken |= (D3DSP_WRITEMASK_0 | D3DSP_WRITEMASK_1 | D3DSP_WRITEMASK_2);
				break;
			case 0x9: // Just .xw is written, we need to add .yz
				*(DWORD* const)writeOperation.registerParameterToken |= (D3DSP_WRITEMASK_1 | D3DSP_WRITEMASK_2);
				break;
			case 0xA: // Just .yw is written, we need to add .xz
				*(DWORD* const)writeOperation.registerParameterToken |= (D3DSP_WRITEMASK_0 | D3DSP_WRITEMASK_2);
				break;
			case 0xB: // .xyw is written, only .z is missing
				*(DWORD* const)writeOperation.registerParameterToken |= (D3DSP_WRITEMASK_2);
				break;
			case 0xC: // Just .zw is written, we need to add .xy
				*(DWORD* const)writeOperation.registerParameterToken |= (D3DSP_WRITEMASK_0 | D3DSP_WRITEMASK_1);
				break;
			case 0xD: // .xzw is written, only .y is missing
				*(DWORD* const)writeOperation.registerParameterToken |= (D3DSP_WRITEMASK_1);
				break;
			case 0xE: // .yzw is written, only .x is missing
				*(DWORD* const)writeOperation.registerParameterToken |= (D3DSP_WRITEMASK_0);
				break;
			case 0xF: // Should never be here, but do nothing!
#ifdef _DEBUG
				__debugbreak();
#else
				__assume(0);
#endif
				break;
			}
		}
	}

	outputBytecode.push_back(*(const DWORD* const)&version);
	while (*pFunction != D3DSIO_END) // End token is special
	{
		const instructionToken rawInstructionToken = *(const instructionToken* const)pFunction;
		const D3DSHADER_INSTRUCTION_OPCODE_TYPE opcode = (const D3DSHADER_INSTRUCTION_OPCODE_TYPE)(rawInstructionToken.opcode);

		switch (opcode)
		{
		case D3DSIO_COMMENT:
		{
			const DWORD commentSize = (*pFunction & D3DSI_COMMENTSIZE_MASK) >> D3DSI_COMMENTSIZE_SHIFT;
			outputBytecode.push_back(*pFunction++);

			for (unsigned x = 0; x < commentSize; ++x)
				outputBytecode.push_back(*pFunction++);
		}
			break;
		default:
			goto arrivedAtFirstInstruction;
		}
	}

	// The first opcode after the version token and the comments and debug data:
arrivedAtFirstInstruction:

	// Modify the bytecode by appending input register declaration instructions (skip this step if we already have input registers declared, say from a D3D9-compatible shader):
	if (originalShaderInfo.declaredRegisters.empty() )
	{
		const unsigned numDeclRegisters = declRegisterMapping.size();
		for (unsigned x = 0; x < numDeclRegisters; ++x)
		{
			const vertDeclRegisterMapping& thisRegMapping = declRegisterMapping[x];

			// Optimization: Skip declaring registers which are never used by the vertex shader, even if they are declared in the D3D8 vertex declaration
			// Example: The D3D8.1 SDK has the DolphinVS Sample project. The DolphinTween.vsh shader only uses input registers v0 thru v6, but the
			// declaration ("dwDolphinVertexDecl" in RestoreDeviceObjects() in DolphinVS.cpp) declares nine registers (v0 thru v8).
			if (!(originalShaderInfo.inputRegistersUsedBitmask & (1 << thisRegMapping.inputRegisterIndex) ) )
				continue;

			// Source on the bytecode format for shader DCL instructions comes from: https://msdn.microsoft.com/en-us/library/windows/hardware/ff549176(v=vs.85).aspx
			instructionToken dclInstruction = {0};
			dclInstruction.opcode = D3DSIO_DCL;
			outputBytecode.push_back(*(const DWORD* const)&dclInstruction);

			DWORD dclToken = thisRegMapping.usageType & D3DSP_DCL_USAGE_MASK;
			dclToken |= (thisRegMapping.usageIndex << D3DSP_DCL_USAGEINDEX_SHIFT) & D3DSP_DCL_USAGEINDEX_MASK;
			dclToken |= 0x80000000; // Bit 31 always has to be 1 for a dcl token
			outputBytecode.push_back(dclToken);

			DWORD destParameter = (D3DSPR_INPUT << D3DSP_REGTYPE_SHIFT) & D3DSP_REGTYPE_MASK;
			destParameter |= thisRegMapping.inputRegisterIndex & D3DSP_REGNUM_MASK;
			destParameter |= D3DSP_WRITEMASK_ALL; // TODO: Is this correct if our vertex decl type is less than the dimension of a float4?
			destParameter |= 0x80000000; // Bit 31 always has to be 1 for a dest parameter token
			outputBytecode.push_back(destParameter);
		}
	}

	// Modify the bytecode by appending constant defines (skip this step if we already have constants defined, say from a D3D9-comptible shader):
	if (originalShaderInfo.initialConstantValues.empty() )
	{
		const unsigned numConstRegs = constFloatRegs.size();
		for (unsigned x = 0; x < numConstRegs; ++x)
		{
			const constFloatReg& constReg = constFloatRegs[x];

			// Source on the bytecode format for shader DEF instructions comes from: https://msdn.microsoft.com/en-us/library/windows/hardware/ff552693(v=vs.85).aspx
			instructionToken defInstruction = {0};
			defInstruction.opcode = D3DSIO_DEF;
			outputBytecode.push_back(*(const DWORD* const)&defInstruction);

			DWORD destParameter = (D3DSPR_CONST << D3DSP_REGTYPE_SHIFT) & D3DSP_REGTYPE_MASK;
			destParameter |= D3DSP_WRITEMASK_ALL; // This is undocumented, but apparently DEF destination parameters always have all write-mask channels set
													// Both D3D9 validation and shader disassembly will produce incorrect results if this is not the case!
			destParameter |= constReg.constRegIndex & D3DSP_REGNUM_MASK;
			destParameter |= 0x80000000; // Bit 31 always has to be 1 for a dest parameter token
			outputBytecode.push_back(destParameter);

			outputBytecode.push_back(*(const DWORD* const)&(constReg.initialValue.r) );
			outputBytecode.push_back(*(const DWORD* const)&(constReg.initialValue.g) );
			outputBytecode.push_back(*(const DWORD* const)&(constReg.initialValue.b) );
			outputBytecode.push_back(*(const DWORD* const)&(constReg.initialValue.a) );
		}
	}

	while (*pFunction != D3DSIO_END)
	{
		if ( ( ( (*pFunction) & D3DSI_OPCODE_MASK) == D3DSIO_COMMENT) && (~*pFunction) & 0x80000000) // Bit 31 must be set to 0x0 if this is indeed a COMMENT
		{
			outputBytecode.push_back(*pFunction); // COMMENT token

			const DWORD numCommentDWORDs = (*pFunction & D3DSI_COMMENTSIZE_MASK) >> D3DSI_COMMENTSIZE_SHIFT;

			++pFunction; // Advance to first comment data DWORD

			for (unsigned x = 0; x < numCommentDWORDs; ++x)
			{
				outputBytecode.push_back(*pFunction);
				++pFunction;
			}
			continue;
		}

		dstParameterToken* const dstParameter = (dstParameterToken* const)(pFunction + 1);

		// Fix up difference between D3D8 validator and D3D9 validator:
		// The D3D8 validator relaxes the write mask requirements on the matrix multiply (MAxB) instructions, so we need
		// to make sure that the instructions' write masks are brought up to spec to the D3D9 requirements:
		BYTE fixupWriteMask = 0x00;
		switch (*pFunction)
		{
		case D3DSIO_M3x2:
			if (dstParameter->GetWriteMask() != (0x1 | 0x2) )
			{
				*(DWORD* const)dstParameter &= (~D3DSP_WRITEMASK_ALL); // Clear all write masks
				*(DWORD* const)dstParameter |= (D3DSP_WRITEMASK_0 | D3DSP_WRITEMASK_1); // Set XY write mask
				fixupWriteMask = (0x4 | 0x8);
			}
			break;
		case D3DSIO_M3x3:
		case D3DSIO_M4x3:
			if (dstParameter->GetWriteMask() != (0x1 | 0x2 | 0x4) )
			{
				*(DWORD* const)dstParameter &= (~D3DSP_WRITEMASK_ALL); // Clear all write masks
				*(DWORD* const)dstParameter |= (D3DSP_WRITEMASK_0 | D3DSP_WRITEMASK_1 | D3DSP_WRITEMASK_2); // Set XYZ write mask
				fixupWriteMask = 0x8;
			}
			break;
		case D3DSIO_M3x4:
		case D3DSIO_M4x4:
			if (dstParameter->GetWriteMask() != (0x1 | 0x2 | 0x4 | 0x8) )
			{
				*(DWORD* const)dstParameter |= (D3DSP_WRITEMASK_ALL); // Set XYZW write mask
			}
			break;
		case D3DSIO_EXP:
		case D3DSIO_EXPP:
		case D3DSIO_LOG:
		case D3DSIO_LOGP:
		case D3DSIO_RCP:
		case D3DSIO_RSQ:
			{
				srcParameterToken* srcParameter0 = (srcParameterToken* const)(dstParameter + 1);
				switch (*(DWORD* const)srcParameter0 & D3DSP_SWIZZLE_MASK)
				{
				case D3DSP_REPLICATERED:
				case D3DSP_REPLICATEGREEN:
				case D3DSP_REPLICATEBLUE:
				case D3DSP_REPLICATEALPHA:
					break;
				default:
					*(DWORD* const)srcParameter0 &= (~D3DSP_SWIZZLE_MASK); // Clear the existing bad (non-replicate) swizzle

					// TODO: Is it correct to use .wwww here? Shouldn't we be using one of the channels that are actually know to be used in the source swizzle?
					*(DWORD* const)srcParameter0 |= D3DSP_REPLICATEALPHA; // Replace with the .wwww replicate swizzle
					break;
				}
			}
			break;
		}

		outputBytecode.push_back(*pFunction);

		if (fixupWriteMask > 0)
		{
			srcParameterToken* srcParameter0 = (srcParameterToken* const)(dstParameter + 1);
			srcParameterToken* srcParameter1 = (srcParameterToken* const)(srcParameter0 + 1);

			outputBytecode.push_back(*(const DWORD* const)dstParameter);
			outputBytecode.push_back(*(const DWORD* const)srcParameter0);
			outputBytecode.push_back(*(const DWORD* const)srcParameter1);

			static const unsigned tokenAdvance = 4; // instruction token, dst parameter token, src0 parameter token, src1 parameter token
			pFunction += tokenAdvance;

			// Assemble a new instruction: ADD rN.zwzw rN.xxxx -rN.xxxx or ADD rN.wwww rN.xxxx -rN.xxxx
			// This is needed in the case of a M4x3, M3x3, and M3x2 write mask fixup to assure that all components
			// of the register have been written to (or else we could fail D3D9 validation on a later instruction
			// for trying to read from a register with an undefined value).

			// Note that we're using the ADD instruction here with the NEGATE source modifier on one of the source parameters
			// because trying to use the real SUB instruction seems to fail D3D9 validation for vs_1_1 (even though the
			// documentation mentions that the SUB instruction is valid for vs_1_1)
			outputBytecode.push_back(D3DSIO_ADD);

			dstParameterToken subDstParam = *dstParameter;
			*(DWORD* const)&subDstParam &= (~D3DSP_WRITEMASK_ALL); // Clear all write masks
			*(DWORD* const)&subDstParam |= (fixupWriteMask << 16); // Set our new write mask
			*(DWORD* const)&subDstParam &= (~D3DSHADER_ADDRESSMODE_MASK); // Clear relative addressing bit
			outputBytecode.push_back(*(const DWORD* const)&subDstParam);

			srcParameterToken subSrcParam0 = *(srcParameterToken*)dstParameter;
			*(DWORD* const)&subSrcParam0 &= (~D3DSP_SRCMOD_MASK); // Clear source/result modifiers
			*(DWORD* const)&subSrcParam0 &= (~D3DSHADER_ADDRESSMODE_MASK); // Clear relative addressing bit
			*(DWORD* const)&subSrcParam0 &= (~D3DSP_SWIZZLE_MASK); // Clear the swizzle
			*(DWORD* const)&subSrcParam0 |= D3DSP_REPLICATERED; // Set the swizzle to the .xxxx "replicate swizzle"
			outputBytecode.push_back(*(const DWORD* const)&subSrcParam0);

			srcParameterToken subSrcParam1 = subSrcParam0;
			*(DWORD* const)&subSrcParam1 |= D3DSPSM_NEG;
			outputBytecode.push_back(*(const DWORD* const)&subSrcParam1);

			continue;
		}

		++pFunction;
	}

	instructionToken endToken = {0};
	endToken.opcode = D3DSIO_END;
	outputBytecode.push_back(*(const DWORD* const)&endToken);
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DDevice8Hook::CreateVertexShader(THIS_ CONST DWORD* pDeclaration,CONST DWORD* pFunction,DWORD* pHandle,DWORD Usage)
{
	if (!pHandle)
	{
		return D3DERR_INVALIDCALL;
	}

	if (!pDeclaration)
	{
		// Assume that we always have a valid vertex decl
#ifdef _DEBUG
		MessageBoxA(NULL, "Error: NULL Vertex Decl used!", NULL, MB_OK);
		__debugbreak();
#endif
		return D3DERR_INVALIDCALL;
	}

	HRESULT hr = S_OK;

	std::vector<D3DVERTEXELEMENT9> vertElements9;
	std::vector<constFloatReg> constFloatRegs;
	std::vector<vertDeclRegisterMapping> declRegisterMapping;
	ConvertVertexDecl8to9(pDeclaration, vertElements9, constFloatRegs, declRegisterMapping);

	LPDIRECT3DVERTEXDECLARATION9 decl9 = NULL;
	hr = d3d9dev->CreateVertexDeclaration(&(vertElements9.front() ), &decl9);
	if (FAILED(hr) )
		return hr;

	LPDIRECT3DVERTEXSHADER9 vs9 = NULL;
	if (pFunction)
	{
		std::vector<DWORD> oldShaderBytecodeCopy;
		const DWORD* pFunctionCopy = pFunction;
		while (*pFunctionCopy != D3DSIO_END)
			oldShaderBytecodeCopy.push_back(*pFunctionCopy++);
		oldShaderBytecodeCopy.push_back(*pFunctionCopy); // Copy the END token too

		ShaderInfo originalShaderInfo;
		DisasmAndAnalyzeShader(&(oldShaderBytecodeCopy.front() ), originalShaderInfo
#ifdef _DEBUG
			, "NoneVS_Original"
#endif
			);

		std::vector<DWORD> newShaderBytecode;
		CopyModifyVertShaderBytecode(&(oldShaderBytecodeCopy.front() ), newShaderBytecode, originalShaderInfo, constFloatRegs, declRegisterMapping);

#ifdef _DEBUG
		ShaderInfo newShaderInfo;
		DisasmAndAnalyzeShader(&(newShaderBytecode.front() ), newShaderInfo, "NoneVS_New");
#endif

		hr = d3d9dev->CreateVertexShader(&(newShaderBytecode.front() ), &vs9);
		if (FAILED(hr) )
		{
#ifdef _DEBUG
			HANDLE hF = CreateFileA("failedOriginalVS.vso", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hF == INVALID_HANDLE_VALUE)
				return hr;

			const DWORD* dwReadPtr = pFunction;
			DWORD numBytesWritten = 0;
			while (*dwReadPtr != D3DSIO_END)
				WriteFile(hF, dwReadPtr++, sizeof(DWORD), &numBytesWritten, NULL);
			WriteFile(hF, dwReadPtr, sizeof(DWORD), &numBytesWritten, NULL);

			CloseHandle(hF);

			hF = CreateFileA("failedD3D9VS.vso", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hF == INVALID_HANDLE_VALUE)
				return hr;

			dwReadPtr = &(newShaderBytecode.front() );
			numBytesWritten = 0;
			while (*dwReadPtr != D3DSIO_END)
				WriteFile(hF, dwReadPtr++, sizeof(DWORD), &numBytesWritten, NULL);
			WriteFile(hF, dwReadPtr, sizeof(DWORD), &numBytesWritten, NULL);

			CloseHandle(hF);
#endif
			return hr;
		}
	}

	void* vs8mem = 
#ifdef USE_PAGE_ALLOCATOR
		VirtualAlloc(NULL, sizeof(IDirect3DVertexShader8Hook), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
#else
		malloc(sizeof(IDirect3DVertexShader8Hook) );
#endif
	IDirect3DVertexShader8Hook* vs8 = new (vs8mem) IDirect3DVertexShader8Hook(vs9, decl9, this);

	*pHandle = (const DWORD)vs8;
	return hr;
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DDevice8Hook::SetVertexShader(THIS_ DWORD Handle)
{
	if (Handle < 0xF0000)
	{
		d3d9dev->SetVertexDeclaration(NULL);
		return d3d9dev->SetFVF(Handle);
	}
	else
	{
		IDirect3DVertexShader8Hook* vs8 = (IDirect3DVertexShader8Hook*)Handle;
		d3d9dev->SetVertexDeclaration(vs8->GetUnderlyingVertexDeclaration() );
		vs8->AddRef();
		return d3d9dev->SetVertexShader(vs8->GetUnderlyingVertexShader() );
	}
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DDevice8Hook::GetVertexShader(THIS_ DWORD* pHandle)
{
	if (!pHandle)
		return d3d9dev->GetVertexShader(NULL);

	LPDIRECT3DVERTEXSHADER9 vs9 = NULL;
	HRESULT ret = d3d9dev->GetVertexShader(&vs9);

	if (vs9)
		*pHandle = (const DWORD)vs9;
	else
		return d3d9dev->GetFVF(pHandle);

	return ret;
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DDevice8Hook::SetVertexShaderConstant(THIS_ DWORD StartRegister, CONST void* pConstantData, DWORD Vector4fCount)
{
	return d3d9dev->SetVertexShaderConstantF(StartRegister, (const float* const)pConstantData, Vector4fCount);
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DDevice8Hook::GetVertexShaderConstant(THIS_ DWORD StartRegister, void* pConstantData, DWORD Vector4fCount)
{
	return d3d9dev->GetVertexShaderConstantF(StartRegister, (float* const)pConstantData, Vector4fCount);
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DDevice8Hook::SetStreamSource(THIS_ UINT StreamNumber, IDirect3DVertexBuffer8* pStreamData, UINT Stride)
{
	if (!pStreamData)
		return d3d9dev->SetStreamSource(StreamNumber, NULL, 0, Stride);

	IDirect3DVertexBuffer8Hook* vbhook = dynamic_cast<IDirect3DVertexBuffer8Hook*>(pStreamData);

	return d3d9dev->SetStreamSource(StreamNumber, vbhook ? vbhook->GetUnderlyingVertexBuffer() : NULL, 0, Stride);
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DDevice8Hook::GetStreamSource(THIS_ UINT StreamNumber, IDirect3DVertexBuffer8** ppStreamData, UINT* pStride)
{
	UINT offsetInBytes = 0;
	if (!ppStreamData)
		return d3d9dev->GetStreamSource(StreamNumber, NULL, &offsetInBytes, pStride);

	LPDIRECT3DVERTEXBUFFER9 vb9 = NULL;
	HRESULT ret = d3d9dev->GetStreamSource(StreamNumber, &vb9, &offsetInBytes, pStride);

	IDirect3DVertexBuffer8Hook* vbhook = ObjLookupCreate(vb9, this);
	*ppStreamData = vbhook;

	return ret;
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DDevice8Hook::SetIndices(THIS_ IDirect3DIndexBuffer8* pIndexData, UINT _BaseVertexIndex)
{
	if (!pIndexData)
		return d3d9dev->SetIndices(NULL);

	BaseVertexIndex = _BaseVertexIndex;

	IDirect3DIndexBuffer8Hook* ibhook = dynamic_cast<IDirect3DIndexBuffer8Hook*>(pIndexData);

	return d3d9dev->SetIndices(ibhook ? ibhook->GetUnderlyingIndexBuffer() : NULL);
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DDevice8Hook::GetIndices(THIS_ IDirect3DIndexBuffer8** ppIndexData, UINT* pBaseVertexIndex)
{
	if (pBaseVertexIndex)
		*pBaseVertexIndex = BaseVertexIndex;

	if (!ppIndexData)
		return d3d9dev->GetIndices(NULL);

	LPDIRECT3DINDEXBUFFER9 ib9 = NULL;
	HRESULT ret = d3d9dev->GetIndices(&ib9);

	IDirect3DIndexBuffer8Hook* ibhook = ObjLookupCreate(ib9, this);
	*ppIndexData = ibhook;

	return ret;
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DDevice8Hook::CreatePixelShader(THIS_ CONST DWORD* pFunction, DWORD* pHandle)
{
	if (!pHandle)
		return D3DERR_INVALIDCALL;

	if (!pFunction)
		return d3d9dev->CreatePixelShader(NULL, (LPDIRECT3DPIXELSHADER9*)pHandle);

	std::vector<DWORD> modifiedBytecode;
	const DWORD* pFunctionCopy = pFunction;
	while (*pFunctionCopy != D3DSIO_END)
		modifiedBytecode.push_back(*pFunctionCopy++);
	modifiedBytecode.push_back(*pFunctionCopy); // Copy the END token too

	// Modify the shader minor version to be at least 1.
	// This is because D3D9 does not support ps_1_0, whereas D3D8 does support ps_1_0.
	versionToken* const versionPtr = (versionToken* const)&(modifiedBytecode.front() );
	if (versionPtr->minorVersion < 1)
		versionPtr->minorVersion = 1;

	// TODO: Do we have to do any of the "crazier" shader bytecode modifications to pass shader validation for pixel shaders like
	// we do for vertex shaders? (Destination register masking, source register replicate swizzles, etc.)

#ifdef _DEBUG
	ShaderInfo shaderInfo;
	DisasmAndAnalyzeShader(&(modifiedBytecode.front() ), shaderInfo, "NonePS");
#endif
	LPDIRECT3DPIXELSHADER9 ps9 = NULL;
	HRESULT ret = d3d9dev->CreatePixelShader(&(modifiedBytecode.front() ), &ps9);
	if (FAILED(ret) )
	{
#ifdef _DEBUG
		HANDLE hF = CreateFileA("failedOriginalPS.pso", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hF == INVALID_HANDLE_VALUE)
			return ret;

		const DWORD* dwReadPtr = pFunction;
		DWORD numBytesWritten = 0;
		while (*dwReadPtr != D3DSIO_END)
			WriteFile(hF, dwReadPtr++, sizeof(DWORD), &numBytesWritten, NULL);
		WriteFile(hF, dwReadPtr, sizeof(DWORD), &numBytesWritten, NULL);

		CloseHandle(hF);

		hF = CreateFileA("failedD3D9PS.pso", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hF == INVALID_HANDLE_VALUE)
			return ret;

		dwReadPtr = &(modifiedBytecode.front() );
		numBytesWritten = 0;
		while (*dwReadPtr != D3DSIO_END)
			WriteFile(hF, dwReadPtr++, sizeof(DWORD), &numBytesWritten, NULL);
		WriteFile(hF, dwReadPtr, sizeof(DWORD), &numBytesWritten, NULL);

		CloseHandle(hF);
#endif
		return ret;
	}

	IDirect3DPixelShader8Hook* pshook = ObjLookupCreate(ps9, this);
	pshook->SetOriginalShaderBytecode(pFunction);
	*pHandle = (const DWORD)pshook;

	return ret;
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DDevice8Hook::SetPixelShader(THIS_ DWORD Handle)
{
	IDirect3DPixelShader8Hook* pshook = (IDirect3DPixelShader8Hook*)Handle;
	if (!pshook)
		return d3d9dev->SetPixelShader(NULL);

	return d3d9dev->SetPixelShader(pshook->GetUnderlyingPixelShader() );
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DDevice8Hook::GetPixelShader(THIS_ DWORD* pHandle)
{
	if (!pHandle)
		return d3d9dev->GetPixelShader(NULL);

	LPDIRECT3DPIXELSHADER9 ps9 = NULL;
	HRESULT ret = d3d9dev->GetPixelShader(&ps9);
	if (FAILED(ret) )
		return ret;

	IDirect3DPixelShader8Hook* pshook = ObjLookupCreate(ps9, this);
	*pHandle = (const DWORD)pshook;
	return ret;
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DDevice8Hook::SetPixelShaderConstant(THIS_ DWORD StartRegister, CONST void* pConstantData, DWORD Vector4fCount)
{
	return d3d9dev->SetPixelShaderConstantF(StartRegister, (const float* const)pConstantData, Vector4fCount);
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DDevice8Hook::GetPixelShaderConstant(THIS_ DWORD StartRegister, void* pConstantData, DWORD Vector4fCount)
{
	return d3d9dev->GetPixelShaderConstantF(StartRegister, (float* const)pConstantData, Vector4fCount);
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DDevice8Hook::DrawRectPatch(THIS_ UINT Handle, CONST float* pNumSegs, CONST D3DRECTPATCH_INFO* pRectPatchInfo)
{
	return d3d9dev->DrawRectPatch(Handle, pNumSegs, pRectPatchInfo);
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DDevice8Hook::DrawTriPatch(THIS_ UINT Handle, CONST float* pNumSegs, CONST D3DTRIPATCH_INFO* pTriPatchInfo)
{
	return d3d9dev->DrawTriPatch(Handle, pNumSegs, pTriPatchInfo);
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DDevice8Hook::DeletePatch(THIS_ UINT Handle)
{
	return d3d9dev->DeletePatch(Handle);
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DDevice8Hook::ResourceManagerDiscardBytes(THIS_ DWORD Bytes)
{
	return d3d9dev->EvictManagedResources();
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DDevice8Hook::CreateImageSurface(THIS_ UINT Width,UINT Height,d3d8::D3DFORMAT Format,IDirect3DSurface8** ppSurface)
{
	if (!ppSurface)
		return d3d9dev->CreateOffscreenPlainSurface(Width, Height, ConvertD3DFORMAT_8to9(Format), D3DPOOL_SCRATCH, NULL, NULL);

	LPDIRECT3DSURFACE9 surf9 = NULL;
	HRESULT ret = d3d9dev->CreateOffscreenPlainSurface(Width, Height, ConvertD3DFORMAT_8to9(Format), D3DPOOL_SCRATCH, &surf9, NULL);

	IDirect3DSurface8Hook* surfhook = ObjLookupCreate(surf9, this);
	*ppSurface = surfhook;

	return ret;
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DDevice8Hook::CopyRects(THIS_ IDirect3DSurface8* pSourceSurface,CONST RECT* pSourceRectsArray,UINT cRects,IDirect3DSurface8* pDestinationSurface,CONST POINT* pDestPointsArray)
{
	IDirect3DSurface8Hook* sourceSurfHook = dynamic_cast<IDirect3DSurface8Hook*>(pSourceSurface);
	IDirect3DSurface8Hook* destSurfHook = dynamic_cast<IDirect3DSurface8Hook*>(pDestinationSurface);
	// According to the D3D9 docs for UpdateSurface(), that is the closest function to D3D8's CopyRects(): https://msdn.microsoft.com/en-us/library/bb205857(VS.85).aspx
	HRESULT hr = S_OK;
	for (unsigned x = 0; x < cRects; ++x)
	{
		HRESULT temphr = d3d9dev->UpdateSurface(sourceSurfHook ? sourceSurfHook->GetUnderlyingSurface() : NULL, pSourceRectsArray ? pSourceRectsArray + x : NULL, destSurfHook ? destSurfHook->GetUnderlyingSurface() : NULL, pDestPointsArray ? pDestPointsArray + x : NULL);
		if (FAILED(temphr) )
			hr = temphr;
	}

	return hr;
}

// Documentation: https://msdn.microsoft.com/en-us/library/ms889638.aspx
COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DDevice8Hook::GetFrontBuffer(THIS_ IDirect3DSurface8* pDestSurface)
{
	if (!pDestSurface)
		return d3d9dev->GetFrontBufferData(0, NULL);

	IDirect3DSurface8Hook* surfhook = dynamic_cast<IDirect3DSurface8Hook*>(pDestSurface);

	return d3d9dev->GetFrontBufferData(0, surfhook ? surfhook->GetUnderlyingSurface() : NULL);
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DDevice8Hook::ApplyStateBlock(THIS_ DWORD Token)
{
	if (!Token)
		return D3DERR_INVALIDCALL;
	IDirect3DStateBlock8Hook* StateBlock = (IDirect3DStateBlock8Hook*)Token;

	if (StateBlock->StateBlockType & D3DSBT_VERTEXSTATE)
		BaseVertexIndex = StateBlock->BaseVertexIndex;

	return StateBlock->Apply();
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DDevice8Hook::CaptureStateBlock(THIS_ DWORD Token)
{
	if (!Token)
		return D3DERR_INVALIDCALL;
	IDirect3DStateBlock8Hook* StateBlock = (IDirect3DStateBlock8Hook*)Token;

	if (StateBlock->StateBlockType & D3DSBT_VERTEXSTATE)
		StateBlock->BaseVertexIndex = BaseVertexIndex;

	return StateBlock->Capture();
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DDevice8Hook::DeleteStateBlock(THIS_ DWORD Token)
{
	if (!Token)
		return D3DERR_INVALIDCALL;
	IDirect3DStateBlock8Hook* StateBlock = (IDirect3DStateBlock8Hook*)Token;
	StateBlock->Release();
	return S_OK;
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DDevice8Hook::GetInfo(THIS_ DWORD DevInfoID,void* pDevInfoStruct,DWORD DevInfoStructSize) 
{
	// Documentation: https://technet.microsoft.com/en-us/ms889650
	if (!pDevInfoStruct)
		return D3DERR_INVALIDCALL;

	if (DevInfoStructSize == 0)
		return D3DERR_INVALIDCALL;

	const DWORD queryStartTime = GetTickCount();

	switch (DevInfoID)
	{
	case D3DDEVINFOID_ZERO:
		return E_FAIL;
	default:
	case D3DDEVINFOID_TEXTUREMANAGER:
	case D3DDEVINFOID_D3DTEXTUREMANAGER:
	case D3DDEVINFOID_TEXTURING:
	case D3DDEVINFOID_RESOURCEMANAGER:
	case D3DDEVINFOID_VCACHE:
	case D3DDEVINFOID_VERTEXSTATS:
	{
		LPDIRECT3DQUERY9 newQuery = NULL;
		HRESULT hr = d3d9dev->CreateQuery( (const D3DQUERYTYPE)DevInfoID, &newQuery);
		if (FAILED(hr) )
			return hr;

		newQuery->Issue(D3DISSUE_END);

		while (S_FALSE == newQuery->GetData(pDevInfoStruct, DevInfoStructSize, D3DGETDATA_FLUSH) )
		{
			if (GetTickCount() - queryStartTime > QUERY_TIMEOUT_TIME_MILLISECONDS)
			{
				newQuery->Release();
				return S_FALSE;
			}
		}
	}
		return S_FALSE;
	case D3DDEVINFOID_UNKNOWN:
		return E_FAIL;
	}
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DDevice8Hook::DeleteVertexShader(THIS_ DWORD Handle)
{
	if (!Handle)
		return D3DERR_INVALIDCALL;
	IDirect3DVertexShader8Hook* VertShader = (IDirect3DVertexShader8Hook*)Handle;
	VertShader->Release();
	return S_OK;
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DDevice8Hook::GetVertexShaderDeclaration(THIS_ DWORD Handle,void* pData,DWORD* pSizeOfData)
{
	if (!Handle)
		return D3DERR_INVALIDCALL;

	IDirect3DVertexShader8Hook* VertShader = (IDirect3DVertexShader8Hook*)Handle;
	LPDIRECT3DVERTEXDECLARATION9 decl9 = VertShader->GetUnderlyingVertexDeclaration();
	if (decl9)
	{
		return decl9->GetDeclaration( (D3DVERTEXELEMENT9*)pData, (UINT*)pSizeOfData);
	}
	else
	{
		if (pSizeOfData)
			*pSizeOfData = 0;
		return S_OK;
	}
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DDevice8Hook::GetVertexShaderFunction(THIS_ DWORD Handle,void* pData,DWORD* pSizeOfData)
{
	if (!Handle)
		return D3DERR_INVALIDCALL;

	IDirect3DVertexShader8Hook* VertShader = (IDirect3DVertexShader8Hook*)Handle;
	return VertShader->GetFunction(pData, (UINT*)pSizeOfData);
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DDevice8Hook::DeletePixelShader(THIS_ DWORD Handle)
{
	if (!Handle)
		return D3DERR_INVALIDCALL;

	IDirect3DPixelShader8Hook* pshook = (IDirect3DPixelShader8Hook*)Handle;
	pshook->Release();
	return S_OK;
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IDirect3DDevice8Hook::GetPixelShaderFunction(THIS_ DWORD Handle,void* pData,DWORD* pSizeOfData)
{
	if (!Handle)
		return D3DERR_INVALIDCALL;

	IDirect3DPixelShader8Hook* pshook = (IDirect3DPixelShader8Hook*)Handle;
	return pshook->GetFunction(pData, (UINT*)pSizeOfData);
}

