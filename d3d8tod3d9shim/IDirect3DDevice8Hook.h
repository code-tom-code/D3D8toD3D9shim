#pragma once

#include "IDirect3D8Hook.h"
#include "ObjectMap.h"
#include <map>

struct IDirect3D8Hook;
struct IDirect3DVertexBuffer8Hook;
struct IDirect3DIndexBuffer8Hook;
struct IDirect3DVertexShader8Hook;
struct IDirect3DPixelShader8Hook;
struct IDirect3DVertexDeclaration8Hook;
struct IDirect3DSurface8Hook;
struct IDirect3DSwapChain8Hook;
struct IDirect3DBaseTexture8Hook;
struct IDirect3DTexture8Hook;
struct IDirect3DCubeTexture8Hook;
struct IDirect3DVolumeTexture8Hook;
struct IDirect3DResource8Hook;
struct IDirect3DStateBlock8Hook;
struct IDirect3DVolume8Hook;

__declspec(align(16) ) struct IDirect3DDevice8Hook : public IDirect3DDevice8
{
public:
	IDirect3DDevice8Hook(LPDIRECT3DDEVICE9 _d3d9dev, IDirect3D8Hook* _parentHook) : d3d9dev(_d3d9dev), parentHook(_parentHook), refCount(1), BaseVertexIndex(0)
	{
		InitializeCriticalSection(&cs);

#ifdef _DEBUG
		for (unsigned x = 0; x < ARRAYSIZE(magicVal); ++x)
			magicVal[x] = 0xFFFFFFFF - x;
#endif
	}


	virtual ~IDirect3DDevice8Hook()
	{
		DeleteCriticalSection(&cs);
		ObjDestroy(this);
	}

	/*** IUnknown methods ***/
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE QueryInterface(THIS_ REFIID riid, void** ppvObj) override;
    virtual COM_DECLSPEC_NOTHROW ULONG STDMETHODCALLTYPE AddRef(THIS) override;
    virtual COM_DECLSPEC_NOTHROW ULONG STDMETHODCALLTYPE Release(THIS) override;

    /*** IDirect3DDevice9 methods ***/
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE TestCooperativeLevel(THIS) override;
    virtual COM_DECLSPEC_NOTHROW UINT STDMETHODCALLTYPE GetAvailableTextureMem(THIS) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetDirect3D(THIS_ IDirect3D8** ppD3D8) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetDeviceCaps(THIS_ D3DCAPS8* pCaps) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetDisplayMode(THIS_ D3DDISPLAYMODE* pMode) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetCreationParameters(THIS_ D3DDEVICE_CREATION_PARAMETERS* pParameters) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE SetCursorProperties(THIS_ UINT XHotSpot, UINT YHotSpot, IDirect3DSurface8* pCursorBitmap) override;
    virtual COM_DECLSPEC_NOTHROW void STDMETHODCALLTYPE SetCursorPosition(THIS_ int X, int Y, DWORD Flags) override;
    virtual COM_DECLSPEC_NOTHROW BOOL STDMETHODCALLTYPE ShowCursor(THIS_ BOOL bShow) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE CreateAdditionalSwapChain(THIS_ d3d8::D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DSwapChain8** pSwapChain) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE Reset(THIS_ d3d8::D3DPRESENT_PARAMETERS* pPresentationParameters) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE Present(THIS_ CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetBackBuffer(THIS_ UINT iBackBuffer, D3DBACKBUFFER_TYPE Type, IDirect3DSurface8** ppBackBuffer) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetRasterStatus(THIS_ D3DRASTER_STATUS* pRasterStatus) override;
    virtual COM_DECLSPEC_NOTHROW void STDMETHODCALLTYPE SetGammaRamp(THIS_ DWORD Flags, CONST D3DGAMMARAMP* pRamp) override;
    virtual COM_DECLSPEC_NOTHROW void STDMETHODCALLTYPE GetGammaRamp(THIS_ D3DGAMMARAMP* pRamp) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE CreateTexture(THIS_ UINT Width, UINT Height, UINT Levels, DWORD Usage, d3d8::D3DFORMAT Format, D3DPOOL Pool, IDirect3DTexture8** ppTexture) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE CreateVolumeTexture(THIS_ UINT Width, UINT Height, UINT Depth, UINT Levels, DWORD Usage, d3d8::D3DFORMAT Format, D3DPOOL Pool, IDirect3DVolumeTexture8** ppVolumeTexture) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE CreateCubeTexture(THIS_ UINT EdgeLength, UINT Levels, DWORD Usage, d3d8::D3DFORMAT Format, D3DPOOL Pool, IDirect3DCubeTexture8** ppCubeTexture) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE CreateVertexBuffer(THIS_ UINT Length, DWORD Usage, DWORD FVF, D3DPOOL Pool, IDirect3DVertexBuffer8** ppVertexBuffer) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE CreateIndexBuffer(THIS_ UINT Length, DWORD Usage, d3d8::D3DFORMAT Format, D3DPOOL Pool, IDirect3DIndexBuffer8** ppIndexBuffer) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE CreateRenderTarget(THIS_ UINT Width, UINT Height, d3d8::D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, BOOL Lockable, IDirect3DSurface8** ppSurface) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE CreateDepthStencilSurface(THIS_ UINT Width, UINT Height, d3d8::D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, IDirect3DSurface8** ppSurface) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE UpdateTexture(THIS_ IDirect3DBaseTexture8* pSourceTexture, IDirect3DBaseTexture8* pDestinationTexture) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE SetRenderTarget(THIS_ IDirect3DSurface8* pRenderTarget,IDirect3DSurface8* pNewZStencil) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetRenderTarget(THIS_ IDirect3DSurface8** ppRenderTarget) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetDepthStencilSurface(THIS_ IDirect3DSurface8** ppZStencilSurface) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE BeginScene(THIS) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE EndScene(THIS) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE Clear(THIS_ DWORD Count, CONST D3DRECT* pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE SetTransform(THIS_ D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX* pMatrix) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetTransform(THIS_ D3DTRANSFORMSTATETYPE State, D3DMATRIX* pMatrix) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE MultiplyTransform(THIS_ D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX* pMatrix) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE SetViewport(THIS_ CONST D3DVIEWPORT8* pViewport) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetViewport(THIS_ D3DVIEWPORT8* pViewport) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE SetMaterial(THIS_ CONST D3DMATERIAL8* pMaterial) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetMaterial(THIS_ D3DMATERIAL8* pMaterial) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE SetLight(THIS_ DWORD Index, CONST D3DLIGHT8* Light) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetLight(THIS_ DWORD Index, D3DLIGHT8* Light) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE LightEnable(THIS_ DWORD Index, BOOL Enable) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetLightEnable(THIS_ DWORD Index, BOOL* pEnable) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE SetClipPlane(THIS_ DWORD Index, CONST float* pPlane) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetClipPlane(THIS_ DWORD Index, float* pPlane) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE SetRenderState(THIS_ d3d8::D3DRENDERSTATETYPE State, DWORD Value) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetRenderState(THIS_ d3d8::D3DRENDERSTATETYPE State, DWORD* pValue) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE CreateStateBlock(THIS_ D3DSTATEBLOCKTYPE Type, DWORD* pToken) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE BeginStateBlock(THIS) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE EndStateBlock(THIS_ DWORD* pToken) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE SetClipStatus(THIS_ CONST D3DCLIPSTATUS8* pClipStatus) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetClipStatus(THIS_ D3DCLIPSTATUS8* pClipStatus) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetTexture(THIS_ DWORD Stage, IDirect3DBaseTexture8** ppTexture) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE SetTexture(THIS_ DWORD Stage, IDirect3DBaseTexture8* pTexture) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetTextureStageState(THIS_ DWORD Stage, d3d8::D3DTEXTURESTAGESTATETYPE Type, DWORD* pValue) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE SetTextureStageState(THIS_ DWORD Stage, d3d8::D3DTEXTURESTAGESTATETYPE Type, DWORD Value) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE ValidateDevice(THIS_ DWORD* pNumPasses) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE SetPaletteEntries(THIS_ UINT PaletteNumber, CONST PALETTEENTRY* pEntries) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetPaletteEntries(THIS_ UINT PaletteNumber, PALETTEENTRY* pEntries) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE SetCurrentTexturePalette(THIS_ UINT PaletteNumber) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetCurrentTexturePalette(THIS_ UINT *PaletteNumber) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE DrawPrimitive(THIS_ D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE DrawIndexedPrimitive(THIS_ D3DPRIMITIVETYPE PrimitiveType, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE DrawPrimitiveUP(THIS_ D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE DrawIndexedPrimitiveUP(THIS_ D3DPRIMITIVETYPE PrimitiveType, UINT MinVertexIndex, UINT NumVertices, UINT PrimitiveCount, CONST void* pIndexData, D3DFORMAT IndexDataFormat, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE ProcessVertices(THIS_ UINT SrcStartIndex, UINT DestIndex, UINT VertexCount, IDirect3DVertexBuffer8* pDestBuffer, DWORD Flags) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE CreateVertexShader(THIS_ CONST DWORD* pDeclaration,CONST DWORD* pFunction,DWORD* pHandle,DWORD Usage) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE SetVertexShader(THIS_ DWORD Handle) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetVertexShader(THIS_ DWORD* pHandle) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE SetVertexShaderConstant(THIS_ DWORD StartRegister, CONST void* pConstantData, DWORD Vector4fCount) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetVertexShaderConstant(THIS_ DWORD StartRegister, void* pConstantData, DWORD Vector4fCount) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE SetStreamSource(THIS_ UINT StreamNumber, IDirect3DVertexBuffer8* pStreamData, UINT Stride) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetStreamSource(THIS_ UINT StreamNumber, IDirect3DVertexBuffer8** ppStreamData, UINT* pStride) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE SetIndices(THIS_ IDirect3DIndexBuffer8* pIndexData, UINT BaseVertexIndex) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetIndices(THIS_ IDirect3DIndexBuffer8** ppIndexData, UINT* pBaseVertexIndex) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE CreatePixelShader(THIS_ CONST DWORD* pFunction, DWORD* pHandle) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE SetPixelShader(THIS_ DWORD Handle) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetPixelShader(THIS_ DWORD* pHandle) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE SetPixelShaderConstant(THIS_ DWORD StartRegister, CONST void* pConstantData, DWORD Vector4fCount) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetPixelShaderConstant(THIS_ DWORD StartRegister, void* pConstantData, DWORD Vector4fCount) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE DrawRectPatch(THIS_ UINT Handle, CONST float* pNumSegs, CONST D3DRECTPATCH_INFO* pRectPatchInfo) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE DrawTriPatch(THIS_ UINT Handle, CONST float* pNumSegs, CONST D3DTRIPATCH_INFO* pTriPatchInfo) override;
    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE DeletePatch(THIS_ UINT Handle) override;

	// d3d8 specific functions that don't exist in d3d9 (at least with the same name):
	virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE ResourceManagerDiscardBytes(THIS_ DWORD Bytes) override;
	virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE CreateImageSurface(THIS_ UINT Width,UINT Height,d3d8::D3DFORMAT Format,IDirect3DSurface8** ppSurface) override;
	virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE CopyRects(THIS_ IDirect3DSurface8* pSourceSurface,CONST RECT* pSourceRectsArray,UINT cRects,IDirect3DSurface8* pDestinationSurface,CONST POINT* pDestPointsArray) override;
	virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetFrontBuffer(THIS_ IDirect3DSurface8* pDestSurface) override;
	virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE ApplyStateBlock(THIS_ DWORD Token) override;
	virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE CaptureStateBlock(THIS_ DWORD Token) override;
	virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE DeleteStateBlock(THIS_ DWORD Token) override;
	virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetInfo(THIS_ DWORD DevInfoID,void* pDevInfoStruct,DWORD DevInfoStructSize) override;
	virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE DeleteVertexShader(THIS_ DWORD Handle) override;
	virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetVertexShaderDeclaration(THIS_ DWORD Handle,void* pData,DWORD* pSizeOfData) override;
	virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetVertexShaderFunction(THIS_ DWORD Handle,void* pData,DWORD* pSizeOfData) override;
	virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE DeletePixelShader(THIS_ DWORD Handle) override;
	virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetPixelShaderFunction(THIS_ DWORD Handle,void* pData,DWORD* pSizeOfData) override;

	inline LPDIRECT3DDEVICE9 GetUnderlyingDevice(void) const
	{
		return d3d9dev;
	}

	CRITICAL_SECTION cs;

	// Don't be a jerk and access any of these maps without locking the critical section first!
	std::map<IDirect3DBaseTexture9*, IDirect3DBaseTexture8Hook*> BaseTextures;
	std::map<IDirect3DCubeTexture9*, IDirect3DCubeTexture8Hook*> CubeTextures;
	std::map<IDirect3DIndexBuffer9*, IDirect3DIndexBuffer8Hook*> IndexBuffers;
	std::map<IDirect3DPixelShader9*, IDirect3DPixelShader8Hook*> PixelShaders;
	std::map<IDirect3DResource9*, IDirect3DResource8Hook*> Resources;
	std::map<IDirect3DStateBlock9*, IDirect3DStateBlock8Hook*> StateBlocks;
	std::map<IDirect3DSurface9*, IDirect3DSurface8Hook*> Surfaces;
	std::map<IDirect3DSwapChain9*, IDirect3DSwapChain8Hook*> SwapChains;
	std::map<IDirect3DTexture9*, IDirect3DTexture8Hook*> Textures;
	std::map<IDirect3DVertexBuffer9*, IDirect3DVertexBuffer8Hook*> VertexBuffers;
	std::map<IDirect3DVertexShader9*, IDirect3DVertexShader8Hook*> VertexShaders;
	std::map<IDirect3DVolume9*, IDirect3DVolume8Hook*> Volumes;
	std::map<IDirect3DVolumeTexture9*, IDirect3DVolumeTexture8Hook*> VolumeTextures;

	IDirect3D8Hook* GetParentD3D(void) const
	{
		return parentHook;
	}

protected:
	LPDIRECT3DDEVICE9 d3d9dev;
	IDirect3D8Hook* parentHook;
	unsigned __int64 refCount;

	// Device state from D3D8 that is not in D3D9:
	UINT BaseVertexIndex; // Set in d3d8 via SetIndices (used in D3D9 for Draw* calls)

#ifdef _DEBUG
	DWORD magicVal[1024];
#endif
};
