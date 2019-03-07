#pragma once

#include "d3d9include.h"
#include "d3d8include.h"
#include "StructConverter.h"

REFIID ConvertRIID8to9(REFIID riid8)
{
	if (riid8 == IID_IDirect3D8)
		return IID_IDirect3D9;
	else if (riid8 == IID_IDirect3DDevice8)
		return IID_IDirect3DDevice9;
	else if (riid8 == IID_IDirect3DBaseTexture8)
		return IID_IDirect3DBaseTexture9;
	else if (riid8 == IID_IDirect3DCubeTexture8)
		return IID_IDirect3DCubeTexture9;
	else if (riid8 == IID_IDirect3DIndexBuffer8)
		return IID_IDirect3DIndexBuffer9;
	else if (riid8 == IID_IDirect3DResource8)
		return IID_IDirect3DResource9;
	else if (riid8 == IID_IDirect3DSurface8)
		return IID_IDirect3DSurface9;
	else if (riid8 == IID_IDirect3DSwapChain8)
		return IID_IDirect3DSwapChain9;
	else if (riid8 == IID_IDirect3DTexture8)
		return IID_IDirect3DTexture9;
	else if (riid8 == IID_IDirect3DVertexBuffer8)
		return IID_IDirect3DVertexBuffer9;
	else if (riid8 == IID_IDirect3DVolume8)
		return IID_IDirect3DVolume9;
	else if (riid8 == IID_IDirect3DVolumeTexture8)
		return IID_IDirect3DVolumeTexture9;
	else
		return IID_IUnknown;
}
