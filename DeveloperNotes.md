# Developer Notes

Microsoft has some excellent documentation about the differences between Direct3D8 and Direct3D9 on [this page](https://msdn.microsoft.com/en-us/library/windows/desktop/bb204851(v=vs.85).aspx) about converting your code from D3D8 to D3D9. You should absolutely read this page if you are interested in the major differences between D3D8 and D3D9.

## Shader Differences

Direct3D 8.0 was the first version of DirectX to add support for vertex shaders and pixel shaders with vs_1_0 and ps_1_0. Then in Direct3D 8.0a and 8.1, support for even more versions were added.
Of the list of D3D8-supported shader types, Direct3D 9 only supports a subset (specifically, vs_1_0 and ps_1_0 are *not* supported by D3D9). You can see a table comparing the shader versions supported by each version of Direct3D below:

|**Direct3D Version**   |**7.0**|**8.0**|**8.0a**|**8.1**|**9.0**|**9.0a**|**9.0b**|**9.0c**|**10.0**|**10.1**|**11.0**                           |
|-----------------------|-------|-------|--------|-------|-------|--------|--------|--------|--------|--------|-----------------------------------|
|**Vertex Shaders**     |       |       |        |       |       |        |        |        |        |        |                                   |
|                       |       |vs_1_0 |vs_1_0  |vs_1_0 |       |        |        |        |        |        |                                   |
|                       |       |       |        |vs_1_1 |vs_1_1 |vs_1_1  |vs_1_1  |vs_1_1  |        |        |                                   |
|                       |       |       |        |       |vs_2_0 |vs_2_0  |vs_2_0  |vs_2_0  |        |        |vs_4_0_level_9_0 + vs_4_0_level_9_1|
|                       |       |       |        |       |       |vs_2_a  |vs_2_a  |vs_2_a  |        |        |vs_4_0_level_9_3                   |
|                       |       |       |        |       |       |        |        |vs_3_0  |        |        |                                   |
|                       |       |       |        |       |       |        |        |        |vs_4_0  |vs_4_0  |vs_4_0                             |
|                       |       |       |        |       |       |        |        |        |        |vs_4_1  |vs_4_1                             |
|                       |       |       |        |       |       |        |        |        |        |        |vs_5_0                             |
|**Pixel Shaders**      |       |       |        |       |       |        |        |        |        |        |                                   |
|                       |       |ps_1_0 |ps_1_0  |ps_1_0 |       |        |        |        |        |        |                                   |
|                       |       |       |ps_1_1  |ps_1_1 |ps_1_1 |ps_1_1  |ps_1_1  |ps_1_1  |        |        |                                   |
|                       |       |       |ps_1_2  |ps_1_2 |ps_1_2 |ps_1_2  |ps_1_2  |ps_1_2  |        |        |                                   |
|                       |       |       |ps_1_3  |ps_1_3 |ps_1_3 |ps_1_3  |ps_1_3  |ps_1_3  |        |        |                                   |
|                       |       |       |        |ps_1_4 |ps_1_4 |ps_1_4  |ps_1_4  |ps_1_4  |        |        |                                   |
|                       |       |       |        |       |ps_2_0 |ps_2_0  |ps_2_0  |ps_2_0  |        |        |ps_4_0_level_9_0 + ps_4_0_level_9_1|
|                       |       |       |        |       |       |ps_2_a  |ps_2_a  |ps_2_a  |        |        |ps_4_0_level_9_3                   |
|                       |       |       |        |       |       |        |ps_2_b  |ps_2_b  |        |        |ps_4_0_level_9_3                   |
|                       |       |       |        |       |       |        |        |ps_3_0  |        |        |                                   |
|                       |       |       |        |       |       |        |        |        |ps_4_0  |ps_4_0  |ps_4_0                             |
|                       |       |       |        |       |       |        |        |        |        |ps_4_1  |ps_4_1                             |
|                       |       |       |        |       |       |        |        |        |        |        |ps_5_0                             |
|**Geometry Shaders**   |       |       |        |       |       |        |        |        |        |        |                                   |
|                       |       |       |        |       |       |        |        |        |gs_4_0  |gs_4_0  |gs_4_0                             |
|                       |       |       |        |       |       |        |        |        |        |gs_4_1  |gs_4_1                             |
|                       |       |       |        |       |       |        |        |        |        |        |gs_5_0                             |
|**Compute Shaders**    |       |       |        |       |       |        |        |        |        |        |                                   |
|                       |       |       |        |       |       |        |        |        |cs_4_0  |cs_4_0  |cs_4_0                             |
|                       |       |       |        |       |       |        |        |        |        |cs_4_1  |cs_4_1                             |
|                       |       |       |        |       |       |        |        |        |        |        |cs_5_0                             |
|**Hull Tess Shaders**  |       |       |        |       |       |        |        |        |        |        |                                   |
|                       |       |       |        |       |       |        |        |        |        |        |hs_5_0                             |
|**Domain Tess Shaders**|       |       |        |       |       |        |        |        |        |        |                                   |
|                       |       |       |        |       |       |        |        |        |        |        |ds_5_0                             |

This means that for this shim, when we encounter any vs_1_0 or ps_1_0 shader, we'll need to manually convert it to at least being a proper vs_1_1 or ps_1_1 shader before D3D9 will accept it.
Additionally, the D3D9 API imposes some stricter requirements and validation rules for its shaders than the D3D8 API did, so we'll need to account for those as well.

All of this shader modification happens at the shader bytecode level inside the shim, at the time of CreatePixelShader() or CreateVertexShader() being called.

### Shader Version

It's fairly simple to change the version token (it's always the first token encountered in the shader bytecode stream) from 1_0 to 1_1. The high-level HLSL rules for version tokens can be found [here (vs)](https://docs.microsoft.com/en-us/windows/desktop/direct3dhlsl/vs---vs) and [here (ps)](https://docs.microsoft.com/en-us/windows/desktop/direct3dhlsl/ps---ps) and the bytecode-level struct documentation can be found [here](https://docs.microsoft.com/en-us/windows-hardware/drivers/display/version-token).

### Validation: Output Register Write Masks

In vs_1_0 (and possibly the whole D3D8 validator) it was possible to create vertex shaders that did not write all channels of a vertex shader's output (o#) register (read more about vs_1_1 registers [here](https://docs.microsoft.com/en-us/windows/desktop/direct3dhlsl/dx9-graphics-reference-asm-vs-registers-vs-1-1) ). Unfortunately, this is an error in D3D9 and it will cause shaders to fail validation if they are passed to Direct3D 9's CreateVertexShader() function (interestingly this restriction was lifted in Shader Model 4+ to go back to the old D3D8 validation rule that not all components of output registers have to be initialized).

In order to correct this problem, we use the ShaderAnalysis library to walk the shader bytecode's tokens, and when we find partial writes to the [Output (o#) Registers](https://docs.microsoft.com/en-us/windows/desktop/direct3dhlsl/dx9-graphics-reference-asm-vs-registers-output) (that is, instructions with the output registers as Destination Parameters), we can then modify the [Destination Parameter Token](https://docs.microsoft.com/en-us/windows-hardware/drivers/display/destination-parameter-token)'s partial write mask to include the missing channels. This step is only performed for output registers that have already been detected as having missing channel writes, and since we're only ever writing to the missing channels and not the existing ones, this change won't stomp valid data if the shader writes into the same output register multiple times from different instructions.

### Validation: Input Register Dcl's

Direct3D 9 [requires](https://docs.microsoft.com/en-us/windows/desktop/direct3dhlsl/dx9-graphics-reference-asm-vs-registers-vs-1-1) that all vertex shader [Input (v#) Registers](https://docs.microsoft.com/en-us/windows/desktop/direct3dhlsl/dx9-graphics-reference-asm-vs-registers-input) are Declared in the vertex shader (using the [DCL pseudoinstruction](https://docs.microsoft.com/en-us/windows/desktop/direct3dhlsl/dcl-usage-input-register---vs)). [DCL](https://docs.microsoft.com/en-us/windows/desktop/direct3dhlsl/dcl-usage-input-register---vs) is very strange, as it appears to act like instruction, but it is actually has [its own bytecode format](https://docs.microsoft.com/en-us/windows-hardware/drivers/display/dcl-instruction) separate from the usual instruction opcode format. Either vs_1_0 or D3D8 (or both) did not require these declarations to be present, and so because there are some vertex shaders without input register declarations we need to insert new [DCL Tokens](https://docs.microsoft.com/en-us/windows-hardware/drivers/display/dcl-instruction) into the shader bytecode stream (after using ShaderAnalysis to detect when input registers are missing a declaration).

### Validation: Replicate Swizzles

Some instructions require the use of "replicate swizzles" (that is, they only accept input Source Parameters in the form of .xxxx, .yyyy, .zzzz, or .wwww). Of these instructions, there are six that D3D8 does not validate the replicate swizzle for, but D3D9 does (and will fail validation if the replicate swizzle is not used on their sole Source Parameter). These instructions are:
* [exp](https://docs.microsoft.com/en-us/windows/desktop/direct3dhlsl/exp---vs)
* [expp](https://docs.microsoft.com/en-us/windows/desktop/direct3dhlsl/expp---vs)
* [log](https://docs.microsoft.com/en-us/windows/desktop/direct3dhlsl/log---vs)
* [logp](https://docs.microsoft.com/en-us/windows/desktop/direct3dhlsl/logp---vs)
* [rcp](https://docs.microsoft.com/en-us/windows/desktop/direct3dhlsl/rcp---vs)
* [rsq](https://docs.microsoft.com/en-us/windows/desktop/direct3dhlsl/rsq---vs)

To fix this issue, we modify the source swizzle on the [Source Parameter Token](https://docs.microsoft.com/en-us/windows-hardware/drivers/display/source-parameter-token) in the shader bytecode for these instructions (if the instruction does not already use a replicate swizzle).

### Validation: Matrix Destination Write Masks

The D3D8 validator relaxes the write mask requirements on the matrix multiply (mAxB) instructions (D3D8 allows arbitrary write masks to all matrix instruction destination registers, whereas D3D9 has specific write mask requirements for each instruction), so we need to make sure that the instructions' write masks are set exactly to spec to the D3D9 requirements.

|Instruction|D3D9 Required Dst Write Mask|
|-----------|----------------------------|
|[m3x2](https://docs.microsoft.com/en-us/windows/desktop/direct3dhlsl/m3x2---vs)       |.xy                         |
|[m3x3](https://docs.microsoft.com/en-us/windows/desktop/direct3dhlsl/m3x3---vs)       |.xyz                        |
|[m3x4](https://docs.microsoft.com/en-us/windows/desktop/direct3dhlsl/m3x4---vs)       |.xyzw                       |
|[m4x3](https://docs.microsoft.com/en-us/windows/desktop/direct3dhlsl/m4x3---vs)       |.xyz                        |
|[m4x4](https://docs.microsoft.com/en-us/windows/desktop/direct3dhlsl/m4x4---vs)       |.xyzw                       |

It's simple enough to change the [Destination Parameter Token](https://docs.microsoft.com/en-us/windows-hardware/drivers/display/destination-parameter-token)'s Write Mask to have the proper write mask as per the above table. However, doing so introduces a new problem: In the case that we "shrink" a write mask (for example from .xyzw in D3D8 for a [m3x2](https://docs.microsoft.com/en-us/windows/desktop/direct3dhlsl/m3x2---vs) instruction to be the proper .xy in D3D9), it's now possible that we've caused some [Temporary (r#) Registers](https://docs.microsoft.com/en-us/windows/desktop/direct3dhlsl/dx9-graphics-reference-asm-vs-registers-temporary) or [Output (o#) Registers](https://docs.microsoft.com/en-us/windows/desktop/direct3dhlsl/dx9-graphics-reference-asm-vs-registers-output) to have uninitialized components (which would fail validation if these components later get read or written into an [Output (o#) Register](https://docs.microsoft.com/en-us/windows/desktop/direct3dhlsl/dx9-graphics-reference-asm-vs-registers-output)). So in order to work around this new issue, we detect this case and then assemble new instructions immediately after the offending (newly shrunk dest write mask) instruction. Specifically, we want to use a completely non-destructive instruction that dirties the register's components but doesn't require new constant registers or new temporary registers for storing and then restoring the value already present in that register. I ended up selecting this instruction:

`ADD rN.zwzw rN.xxxx -rN.xxxx`

It's an [ADD instruction](https://docs.microsoft.com/en-us/windows/desktop/direct3dhlsl/add---vs) with a self-negate (which is legal HLSL bytecode insofar as I can tell). This causes the register to be added to itself, and then subtracted from itself, which undoes the write but the D3D9 validator considers the register as properly "dirtied". As a working example:

TODO: Create a working example

### Immediate Constants

Todo: Discuss how D3D8 passes immediate constants in its "vertex declaration" bytecode and how we parse this and then insert it into the shader as DEF tokens.
