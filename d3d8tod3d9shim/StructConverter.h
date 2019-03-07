#pragma once

// Changes from d3d8 to d3d9 are described in this MSDN doc: https://msdn.microsoft.com/en-us/library/windows/desktop/bb204851(v=vs.85).aspx

// Different types between d3d8 and d3d9:
// D3DSURFACE_DESC
// D3DVOLUME_DESC
// D3DPRESENT_PARAMETERS

// Unsupported render states from D3D8 that don't exist in D3D9:
// D3DRS_LINEPATTERN (10)
// D3DRS_ZVISIBLE (30)
// D3DRS_EDGEANTIALIAS (40)
// D3DRS_ZBIAS(47)
// D3DRS_SOFTWAREVERTEXPROCESSING (153)
// D3DRS_PATCHSEGMENTS (164)

// D3DLINEPATTERN struct from D3D8 is gone

// Change to D3DFVF_POSITION_MASK (D3D9 added a new high-bit for D3DFVF_XYZW)
// D3D8: D3DFVF_POSITION_MASK = 0x000E
// D3D9: D3DFVF_POSITION_MASK = 0x400E

// Vertex shader function declarations from D3D8 are gone and replaced with vertex declarations in D3D9 (see description in the comments of d3d8types.h)

// D3DBASIS_INTERPOLATE is now D3DBASIS_CATMULL_ROM (there is a comment about this in d3d9types.h)

// D3DFMT_W11V11U10 (65)

// D3DSWAPEFFECT_COPY_VSYNC (overwritten with the d3d9ex swap effect D3DSWAPEFFECT_OVERLAY, which acts differently)
// D3DPRESENT_RATE_UNLIMITED (0x7fffffff) was removed?

// Caps flags changes:
// D3DCAPS2_NO2DDURING3DSCENE 0x00000002L
// D3DCAPS2_CANRENDERWINDOWED 0x00080000L
// D3DPMISCCAPS_LINEPATTERNREP
// D3DPRASTERCAPS_PAT
// D3DVTXPCAPS_NO_VSDT_UBYTE4

static inline void ConvertD3DADAPTER_IDENTIFIER9to8(D3DADAPTER_IDENTIFIER8& outIdentifier, const D3DADAPTER_IDENTIFIER9& inIdentifier)
{
	memcpy(&outIdentifier.Driver, &inIdentifier.Driver, sizeof(outIdentifier.Driver) );
	memcpy(&outIdentifier.Description, &inIdentifier.Description, sizeof(outIdentifier.Description) );
	// Skip the DeviceName member which only exists in the 9 version

#ifdef _WIN32
	outIdentifier.DriverVersion = inIdentifier.DriverVersion;
#else
	outIdentifier.DriverVersionLowPart = inIdentifier.DriverVersionLowPart;
	outIdentifier.DriverVersionHighPart = inIdentifier.DriverVersionHighPart;
#endif

	outIdentifier.VendorId = inIdentifier.VendorId;
	outIdentifier.DeviceId = inIdentifier.DeviceId;
	outIdentifier.SubSysId = inIdentifier.SubSysId;
	outIdentifier.Revision = inIdentifier.Revision;

	outIdentifier.DeviceIdentifier = inIdentifier.DeviceIdentifier;

	outIdentifier.WHQLLevel = inIdentifier.WHQLLevel;
}

static inline void ConvertD3DCAPS9to8(const D3DCAPS9& inCaps, D3DCAPS8& outCaps)
{
	// Thank goodness the D3D9 caps struct is exactly the same as the D3D8 caps struct with a bunch of
	// new stuff we can ignore stapled onto the end!
	memcpy(&outCaps, &inCaps, sizeof(outCaps) );

	// We can always render in Windowed mode
	// This CAPS was removed in D3D9 because it's assumed to always be "true"
	outCaps.Caps2 |= D3DCAPS2_CANRENDERWINDOWED;

	// We do not support line pattern repeats (this feature was removed from D3D9)
	outCaps.PrimitiveMiscCaps &= ~(D3DPMISCCAPS_LINEPATTERNREP);
	
	// This CAPS bit was removed from D3D9, and with modern GPUs it should never be set
	outCaps.PrimitiveMiscCaps &= ~(D3DCAPS2_NO2DDURING3DSCENE);

	// This is always supported because UBYTE4 is required for D3D9
	outCaps.VertexProcessingCaps &= ~(D3DVTXPCAPS_NO_VSDT_UBYTE4);

	// Mod the shader versions down so that D3D8 doesn't get confused (even though we technically support higher than this)
	// TES3: Morrowind's launcher will get confused if it sees a pixel shader version higher than 1.4 (and assume we don't support pixel shading)
	outCaps.VertexShaderVersion = D3DVS_VERSION(1, 1);
	outCaps.PixelShaderVersion = D3DPS_VERSION(1, 4);

	// Remove caps that don't exist in D3D8:
	outCaps.Caps &= ~(D3DCAPS_OVERLAY);
	outCaps.Caps2 &= ~(D3DCAPS2_CANAUTOGENMIPMAP
#ifndef D3D_DISABLE_9EX
		| D3DCAPS2_CANSHARERESOURCE
#endif
		);
	outCaps.Caps3 &= ~(D3DCAPS3_LINEAR_TO_SRGB_PRESENTATION | D3DCAPS3_COPY_TO_VIDMEM | D3DCAPS3_COPY_TO_SYSTEMMEM | D3DCAPS3_DXVAHD);
	outCaps.PrimitiveMiscCaps &= ~(D3DPMISCCAPS_INDEPENDENTWRITEMASKS | D3DPMISCCAPS_PERSTAGECONSTANT | D3DPMISCCAPS_FOGANDSPECULARALPHA | D3DPMISCCAPS_SEPARATEALPHABLEND | 
		D3DPMISCCAPS_MRTINDEPENDENTBITDEPTHS | D3DPMISCCAPS_MRTPOSTPIXELSHADERBLENDING | D3DPMISCCAPS_FOGVERTEXCLAMPED
#ifndef D3D_DISABLE_9EX
		| D3DPMISCCAPS_POSTBLENDSRGBCONVERT
#endif // D3D_DISABLE_9EX
		);
	outCaps.LineCaps &= ~(D3DLINECAPS_ANTIALIAS);
	outCaps.SrcBlendCaps &= ~(D3DPBLENDCAPS_BLENDFACTOR
#ifndef D3D_DISABLE_9EX
		| D3DPBLENDCAPS_SRCCOLOR2 | D3DPBLENDCAPS_INVSRCCOLOR2
#endif // D3D_DISABLE_9EX
		);
	outCaps.DestBlendCaps &= ~(D3DPBLENDCAPS_BLENDFACTOR
#ifndef D3D_DISABLE_9EX
		| D3DPBLENDCAPS_SRCCOLOR2 | D3DPBLENDCAPS_INVSRCCOLOR2
#endif // D3D_DISABLE_9EX
		);
	outCaps.TextureCaps &= ~(D3DPTEXTURECAPS_NOPROJECTEDBUMPENV);
	outCaps.TextureFilterCaps &= ~(D3DPTFILTERCAPS_MINFPYRAMIDALQUAD | D3DPTFILTERCAPS_MINFGAUSSIANQUAD
#ifndef D3D_DISABLE_9EX
		| D3DPTFILTERCAPS_CONVOLUTIONMONO
#endif // D3D_DISABLE_9EX
		);
	outCaps.StencilCaps &= ~(D3DSTENCILCAPS_TWOSIDED);
	outCaps.VertexProcessingCaps &= ~(D3DVTXPCAPS_TEXGEN_SPHEREMAP | D3DVTXPCAPS_NO_TEXGEN_NONLOCALVIEWER);

	// Remove raster caps bits from D3D9:
	outCaps.RasterCaps &= ~(D3DPRASTERCAPS_SCISSORTEST | D3DPRASTERCAPS_SLOPESCALEDEPTHBIAS | D3DPRASTERCAPS_DEPTHBIAS | D3DPRASTERCAPS_MULTISAMPLE_TOGGLE);

	// Fake set raster caps for D3D8:
	outCaps.RasterCaps &= ~(D3DPRASTERCAPS_PAT); // This feature doesn't exist in D3D9, so we don't support it
	outCaps.RasterCaps &= ~(D3DPRASTERCAPS_ANTIALIASEDGES);
	outCaps.RasterCaps |= (D3DPRASTERCAPS_ZBIAS);
	outCaps.RasterCaps &= ~(D3DPRASTERCAPS_STRETCHBLTMULTISAMPLE);
}

static inline const D3DFORMAT ConvertD3DFORMAT_8to9(const d3d8::D3DFORMAT fmt)
{
	switch (fmt)
	{
	case d3d8::D3DFMT_W11V11U10:
		return D3DFMT_UNKNOWN;
	default:
		return (const D3DFORMAT)fmt;
	}
}

static inline const d3d8::D3DFORMAT ConvertD3DFORMAT_9to8(const D3DFORMAT fmt)
{
	switch (fmt)
	{
	case D3DFMT_A8B8G8R8:
		return d3d8::D3DFMT_A8R8G8B8; // Worst-case, the RGB colors will be reversed
	case D3DFMT_X8B8G8R8:
		return d3d8::D3DFMT_X8R8G8B8; // Worst-case, the RGB colors will be reversed
	case D3DFMT_A2R10G10B10:
		return d3d8::D3DFMT_A2B10G10R10; // Worst-case, the RGB colors will be reversed
	case D3DFMT_D24FS8:
		return d3d8::D3DFMT_D24S8; // Worst-case there may be some depth banding artifacts from FP-precision issues
	case D3DFMT_A16B16G16R16:
	case D3DFMT_R8G8_B8G8:
	case D3DFMT_G8R8_G8B8:
	case D3DFMT_D32F_LOCKABLE:
#if !defined(D3D_DISABLE_9EX)
	case D3DFMT_D32_LOCKABLE:
	case D3DFMT_S8_LOCKABLE:
	case D3DFMT_A1:
	case D3DFMT_A2B10G10R10_XR_BIAS:
	case D3DFMT_BINARYBUFFER:
#endif // #if !defined(D3D_DISABLE_9EX)
	case D3DFMT_L16:
	case D3DFMT_Q16W16V16U16:
	case D3DFMT_MULTI2_ARGB8:
	case D3DFMT_R16F:
	case D3DFMT_G16R16F:
	case D3DFMT_A16B16G16R16F:
	case D3DFMT_R32F:
	case D3DFMT_G32R32F:
	case D3DFMT_A32B32G32R32F:
	case D3DFMT_CxV8U8:
		return d3d8::D3DFMT_UNKNOWN; // Can't convert
	default:
		return (const d3d8::D3DFORMAT)fmt;
	}
}

static inline const d3d8::D3DMULTISAMPLE_TYPE ConvertD3DMULTISAMPLE_TYPE9to8(const D3DMULTISAMPLE_TYPE type)
{
	if (type == D3DMULTISAMPLE_NONMASKABLE)
		return d3d8::D3DMULTISAMPLE_2_SAMPLES;
	return (const d3d8::D3DMULTISAMPLE_TYPE)type;
}

static inline const void ConvertD3DMULTISAMPLE_TYPE8to9(const d3d8::D3DMULTISAMPLE_TYPE type, D3DMULTISAMPLE_TYPE& outType9, DWORD& outQuality9)
{
	outType9 = (const D3DMULTISAMPLE_TYPE)type;
	outQuality9 = 0;
}

static inline const DWORD ConvertD3DPRESENTFLAGS9to8(const DWORD flags9)
{
	// In d3d8, the only valid D3DPRESENTFLAG is LOCKABLE_BACKBUFFER:
	return flags9 & D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
}

static inline const DWORD ConvertD3DPRESENTFLAGS8to9(const DWORD flags8)
{
	// In d3d8, the only valid D3DPRESENTFLAG is LOCKABLE_BACKBUFFER:
	return flags8 & D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
}

static inline const d3d8::D3DSWAPEFFECT ConvertD3DSWAPEFFECT9to8(const D3DSWAPEFFECT swap9)
{
	switch (swap9)
	{
	default:
	case D3DSWAPEFFECT_DISCARD:
	case D3DSWAPEFFECT_FLIP:
	case D3DSWAPEFFECT_COPY:
		return (const d3d8::D3DSWAPEFFECT)swap9;
#if !defined(D3D_DISABLE_9EX)
	case D3DSWAPEFFECT_OVERLAY:
	case D3DSWAPEFFECT_FLIPEX:
		return d3d8::D3DSWAPEFFECT_DISCARD;
#endif // #if !defined(D3D_DISABLE_9EX)
	}
}

static inline const D3DSWAPEFFECT ConvertD3DSWAPEFFECT8to9(const d3d8::D3DSWAPEFFECT swap8)
{
	switch (swap8)
	{
	case d3d8::D3DSWAPEFFECT_COPY_VSYNC: // Removed in d3d9 as described in: https://msdn.microsoft.com/en-us/library/windows/desktop/bb204851(v=vs.85).aspx
		return D3DSWAPEFFECT_COPY; // This mode is most similar to D3DSWAPEFFECT_COPY, but actually behaves a little bit differently for windowed applications (we'll just ignore that difference)
	default:
		return (const D3DSWAPEFFECT)swap8;
	}
}

static inline void ConvertD3DPRESENT_PARAMETERS9to8(const D3DPRESENT_PARAMETERS& in9parameters, d3d8::D3DPRESENT_PARAMETERS& out8parameters)
{
	out8parameters.BackBufferWidth = in9parameters.BackBufferWidth;
	out8parameters.BackBufferHeight = in9parameters.BackBufferHeight;
	out8parameters.BackBufferFormat = ConvertD3DFORMAT_9to8(in9parameters.BackBufferFormat);
	out8parameters.BackBufferCount = in9parameters.BackBufferCount;
	out8parameters.MultiSampleType = ConvertD3DMULTISAMPLE_TYPE9to8(in9parameters.MultiSampleType);
	out8parameters.SwapEffect = ConvertD3DSWAPEFFECT9to8(in9parameters.SwapEffect);
	out8parameters.hDeviceWindow = in9parameters.hDeviceWindow;
	out8parameters.Windowed = in9parameters.Windowed;
	out8parameters.EnableAutoDepthStencil = in9parameters.EnableAutoDepthStencil;
	out8parameters.AutoDepthStencilFormat = ConvertD3DFORMAT_9to8(in9parameters.AutoDepthStencilFormat);
	out8parameters.Flags = ConvertD3DPRESENTFLAGS9to8(in9parameters.Flags);
	out8parameters.FullScreen_RefreshRateInHz = in9parameters.FullScreen_RefreshRateInHz;
	out8parameters.FullScreen_PresentationInterval = in9parameters.PresentationInterval;
}

static inline void ConvertD3DPRESENT_PARAMETERS8to9(const d3d8::D3DPRESENT_PARAMETERS& in8parameters, D3DPRESENT_PARAMETERS& out9parameters)
{
	out9parameters.BackBufferWidth = in8parameters.BackBufferWidth;
	out9parameters.BackBufferHeight = in8parameters.BackBufferHeight;
	out9parameters.BackBufferFormat = ConvertD3DFORMAT_8to9(in8parameters.BackBufferFormat);
	out9parameters.BackBufferCount = in8parameters.BackBufferCount;
	ConvertD3DMULTISAMPLE_TYPE8to9(in8parameters.MultiSampleType, out9parameters.MultiSampleType, out9parameters.MultiSampleQuality);
	out9parameters.SwapEffect = ConvertD3DSWAPEFFECT8to9(in8parameters.SwapEffect);
	out9parameters.hDeviceWindow = in8parameters.hDeviceWindow;
	out9parameters.Windowed = in8parameters.Windowed;
	out9parameters.EnableAutoDepthStencil = in8parameters.EnableAutoDepthStencil;
	out9parameters.AutoDepthStencilFormat = ConvertD3DFORMAT_8to9(in8parameters.AutoDepthStencilFormat);
	out9parameters.Flags = ConvertD3DPRESENTFLAGS8to9(in8parameters.Flags);
	out9parameters.FullScreen_RefreshRateInHz = in8parameters.FullScreen_RefreshRateInHz;

	if (in8parameters.FullScreen_PresentationInterval == D3DPRESENT_RATE_UNLIMITED)
		out9parameters.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	else
		out9parameters.PresentationInterval = in8parameters.FullScreen_PresentationInterval;
}

static inline const unsigned GetSizeFromFormat(const d3d8::D3DFORMAT fmt)
{
	switch (fmt)
	{
	default:
	case d3d8::D3DFMT_UNKNOWN:
	case d3d8::D3DFMT_A8R8G8B8:
	case d3d8::D3DFMT_X8R8G8B8:
	case d3d8::D3DFMT_A2B10G10R10:
	case d3d8::D3DFMT_X8L8V8U8:             
	case d3d8::D3DFMT_Q8W8V8U8:             
	case d3d8::D3DFMT_V16U16:               
	case d3d8::D3DFMT_G16R16:              
	case d3d8::D3DFMT_W11V11U10:             // Note: This format does not exist in d3d9!
	case d3d8::D3DFMT_A2W10V10U10:          
	case d3d8::D3DFMT_D32:                 
	case d3d8::D3DFMT_D24X8:                
	case d3d8::D3DFMT_D24S8:                
	case d3d8::D3DFMT_D24X4S4:              
	case d3d8::D3DFMT_INDEX32:              
	case d3d8::D3DFMT_UYVY:                 
	case d3d8::D3DFMT_YUY2:                 
		return 4;
	case d3d8::D3DFMT_R8G8B8:
		return 3;
	case d3d8::D3DFMT_R5G6B5:
	case d3d8::D3DFMT_X1R5G5B5:
	case d3d8::D3DFMT_A1R5G5B5:
	case d3d8::D3DFMT_A4R4G4B4:
	case d3d8::D3DFMT_A8R3G3B2:             
	case d3d8::D3DFMT_X4R4G4B4:             
	case d3d8::D3DFMT_A8P8:                 
	case d3d8::D3DFMT_A8L8:                 
	case d3d8::D3DFMT_V8U8:                 
	case d3d8::D3DFMT_L6V5U5:              
	case d3d8::D3DFMT_D16_LOCKABLE:         
	case d3d8::D3DFMT_D15S1:                
	case d3d8::D3DFMT_D16:                 
	case d3d8::D3DFMT_INDEX16:             
		return 2;
	case d3d8::D3DFMT_R3G3B2:               
	case d3d8::D3DFMT_A8:                   
	case d3d8::D3DFMT_P8:                   
	case d3d8::D3DFMT_L8:                   
	case d3d8::D3DFMT_A4L4:                 
	case d3d8::D3DFMT_VERTEXDATA:            
		return 1;
	case d3d8::D3DFMT_DXT1:                 
		return 1;
	case d3d8::D3DFMT_DXT2:                 
	case d3d8::D3DFMT_DXT3:                 
	case d3d8::D3DFMT_DXT4:                 
	case d3d8::D3DFMT_DXT5:                
		return 2;
	}
}

static inline const DWORD ConvertD3DUSAGE9to8(const DWORD usage9)
{
	static const unsigned validUsages8 = D3DUSAGE_RENDERTARGET | D3DUSAGE_DEPTHSTENCIL | D3DUSAGE_WRITEONLY | D3DUSAGE_SOFTWAREPROCESSING | D3DUSAGE_DONOTCLIP | D3DUSAGE_POINTS | D3DUSAGE_RTPATCHES | D3DUSAGE_NPATCHES | D3DUSAGE_DYNAMIC;
	return usage9 & validUsages8;
}

static inline void ConvertD3DSURFACE_DESC9to8(const D3DSURFACE_DESC& in9desc, d3d8::D3DSURFACE_DESC& out8desc)
{
	out8desc.Format = ConvertD3DFORMAT_9to8(in9desc.Format);
	out8desc.Type = (const d3d8::D3DRESOURCETYPE)in9desc.Type;
	out8desc.Usage = ConvertD3DUSAGE9to8(in9desc.Usage);
	out8desc.Pool = (const d3d8::D3DPOOL)in9desc.Pool;

	out8desc.Size = GetSizeFromFormat(out8desc.Format) * in9desc.Width * in9desc.Height;

	out8desc.MultiSampleType = ConvertD3DMULTISAMPLE_TYPE9to8(in9desc.MultiSampleType);
	out8desc.Width = in9desc.Width;
	out8desc.Height = in9desc.Height;
}

static inline void ConvertD3DVOLUME_DESC9to8(const D3DVOLUME_DESC& in9desc, d3d8::D3DVOLUME_DESC& out8desc)
{
	out8desc.Format = ConvertD3DFORMAT_9to8(in9desc.Format);
	out8desc.Type = (const d3d8::D3DRESOURCETYPE)in9desc.Type;
	out8desc.Usage = ConvertD3DUSAGE9to8(in9desc.Usage);
	out8desc.Pool = (const d3d8::D3DPOOL)in9desc.Pool;

	out8desc.Size = GetSizeFromFormat(out8desc.Format) * in9desc.Width * in9desc.Height * in9desc.Depth;

	out8desc.Width = in9desc.Width;
	out8desc.Height = in9desc.Height;
	out8desc.Depth = in9desc.Depth;
}

REFIID ConvertRIID8to9(REFIID riid8);
