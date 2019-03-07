#pragma once

#pragma pack(push)

// The intention of this class is to have a float4 class with no constructor overhead. Do not add a constructor to this struct that does anything!
struct float4
{
	/*float4(const float _x, const float _y, const float _z, const float _w) : x(_x), y(_y), z(_z), w(_w)
	{
	}*/

	float x, y, z, w;
};

// The intention of this class is to have an int4 class with no constructor overhead. Do not add a constructor to this struct that does anything!
struct int4
{
	int4()
	{
	}

	int4(const int _x, const int _y, const int _z, const int _w) : x(_x), y(_y), z(_z), w(_w)
	{
	}

	int x, y, z, w;
};

struct versionToken
{
	unsigned char minorVersion;
	unsigned char majorVersion;
	unsigned short pixelOrVertexShader;
};
static_assert(sizeof(versionToken) == sizeof(DWORD), "Error!");

// Instruction Token: https://msdn.microsoft.com/en-us/library/windows/hardware/ff567721(v=vs.85).aspx
struct instructionToken
{
	unsigned short opcode; // Bits 0 through 15 indicate an operation code. D3DSIO_* is an example of an operation code, where * represents the instruction.
	unsigned char opcodeControls; // Bits 16 through 23 indicate specific controls related to the operation code.
	unsigned char instructionLength : 4; /* For pixel and vertex shader versions earlier than 2_0, bits 24 through 27 are reserved and set to 0x0.
										 For pixel and vertex shader versions 2_0 and later, bits 24 through 27 specify the size in DWORDs of the 
										 instruction excluding the instruction token itself (that is, the number of tokens that comprise the 
										 instruction excluding the instruction token). */
	unsigned char predicated : 1; /* For pixel and vertex shader versions earlier than 2_0, bit 28 is reserved and set to 0x0.
								  For pixel and vertex shader versions 2_0 and later, bit 28 indicates whether the instruction is predicated (that is, 
								  contains an extra predicate source token at the end of the shader code. If this 
								  bit is set to 0x1, the instruction is predicated. */
	unsigned char reservedZero : 1; // Reserved. This value is set to 0x0.
	unsigned char coIssue : 1; /* For pixel shader versions earlier than 2_0, bit 30 is the co-issue bit.
							   If set to 1, execute this instruction with previous instructions; otherwise, execute separately.
							   For pixel shader version 2_0 and later and all vertex shader versions, bit 30 is reserved and set to 0x0. */
	unsigned char instructionMarkerBit : 1; // Always 0x0 for instruction tokens
};
static_assert(sizeof(instructionToken) == sizeof(DWORD), "Error!");

struct parameterToken
{
	inline const unsigned GetRegisterIndex(void) const
	{
		return internal & D3DSP_REGNUM_MASK;
	}

	inline const D3DSHADER_PARAM_REGISTER_TYPE GetRegisterType(void) const
	{
		const unsigned regType1 = (internal & D3DSP_REGTYPE_MASK) >> D3DSP_REGTYPE_SHIFT;
		const unsigned regType2 = ( (internal & D3DSP_REGTYPE_MASK2) >> D3DSP_REGTYPE_SHIFT2) ;// >> 3;
		return (const D3DSHADER_PARAM_REGISTER_TYPE)(regType1 | regType2);
	}

	inline const D3DSHADER_ADDRESSMODE_TYPE GetRelativeAddressingType(void) const
	{
		return (const D3DSHADER_ADDRESSMODE_TYPE)( (internal & D3DSHADER_ADDRESSMODE_MASK) );
	}

	inline const unsigned char GetSwizzle(void) const
	{
		return (const unsigned char)( (internal & D3DVS_SWIZZLE_MASK) >> D3DVS_SWIZZLE_SHIFT);
	}

	DWORD internal;
};

static_assert(sizeof(parameterToken) == sizeof(DWORD), "Error!");

// Source: https://msdn.microsoft.com/en-us/library/windows/hardware/ff569716(v=vs.85).aspx
struct srcParameterToken : public parameterToken
{
	inline const unsigned GetSourceModifiers(void) const
	{
		return ( (internal & D3DSP_SRCMOD_MASK) >> D3DSP_SRCMOD_SHIFT);
	}

	inline const D3DSHADER_PARAM_SRCMOD_TYPE GetSourceModifiersUnshifted(void) const
	{
		return (const D3DSHADER_PARAM_SRCMOD_TYPE)(internal & D3DSP_SRCMOD_MASK);
	}

	// Returns 0, 1, 2, or 3 to indicate an X, Y, Z, or W swizzle respectively
	inline const unsigned GetChannelSwizzle(void) const
	{
		return ( (internal & D3DSP_SWIZZLE_MASK) >> D3DSP_SWIZZLE_SHIFT) & 0x3;
	}

	inline const unsigned GetChannelSwizzleXYZW(void) const
	{
		return ( (internal & D3DSP_SWIZZLE_MASK) >> D3DSP_SWIZZLE_SHIFT);
	}
};

static_assert(sizeof(srcParameterToken) == sizeof(DWORD), "Error!");

// Source: https://msdn.microsoft.com/en-us/library/windows/hardware/ff552738(v=vs.85).aspx
struct dstParameterToken : public parameterToken
{
	inline const unsigned GetWriteMask(void) const
	{
		return (internal & D3DSP_WRITEMASK_ALL) >> 16;
	}

	inline const unsigned GetResultShiftScale(void) const
	{
		return (internal & D3DSP_DSTSHIFT_MASK) >> D3DSP_DSTSHIFT_SHIFT;
	}

	inline const unsigned GetResultModifier(void) const
	{
		return (internal & D3DSP_DSTMOD_MASK) >> D3DSP_DSTMOD_SHIFT;
	}

	inline const unsigned GetResultModifierUnshifted(void) const
	{
		return (internal & D3DSP_DSTMOD_MASK);
	}
};

static_assert(sizeof(dstParameterToken) == sizeof(DWORD), "Error!");

#pragma pack(pop)
