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

In order to correct this problem, we use the ShaderAnalysis library to walk the shader bytecode's tokens, and when we find partial writes to the [Output (o#) Registers](https://docs.microsoft.com/en-us/windows/desktop/direct3dhlsl/dx9-graphics-reference-asm-vs-registers-output) (that is, instructions with the output registers as Destination Parameters), we can then modify the [Destination Parameter Token](https://docs.microsoft.com/en-us/windows-hardware/drivers/display/destination-parameter-token)'s partial [Write Mask](https://docs.microsoft.com/en-us/windows/desktop/direct3dhlsl/dx9-graphics-reference-asm-vs-registers-modifiers-masking#destination-register-masking-1) to include the missing channels. This step is only performed for output registers that have already been detected as having missing channel writes, and since we're only ever writing to the missing channels and not the existing ones, this change won't stomp valid data if the shader writes into the same output register multiple times from different instructions.

### Validation: Input Register Dcl's

Direct3D 9 [requires](https://docs.microsoft.com/en-us/windows/desktop/direct3dhlsl/dx9-graphics-reference-asm-vs-registers-vs-1-1) that all vertex shader [Input (v#) Registers](https://docs.microsoft.com/en-us/windows/desktop/direct3dhlsl/dx9-graphics-reference-asm-vs-registers-input) are Declared in the vertex shader (using the [DCL pseudoinstruction](https://docs.microsoft.com/en-us/windows/desktop/direct3dhlsl/dcl-usage-input-register---vs)). [DCL](https://docs.microsoft.com/en-us/windows/desktop/direct3dhlsl/dcl-usage-input-register---vs) is very strange, as it appears to act like instruction, but it is actually has [its own bytecode format](https://docs.microsoft.com/en-us/windows-hardware/drivers/display/dcl-instruction) separate from the usual instruction opcode format. Either vs_1_0 or D3D8 (or both) did not require these declarations to be present, and so because there are some vertex shaders without input register declarations we need to insert new [DCL Tokens](https://docs.microsoft.com/en-us/windows-hardware/drivers/display/dcl-instruction) into the shader bytecode stream (after using ShaderAnalysis to detect when input registers are missing a declaration).

### Validation: Replicate Swizzles

Some instructions require the use of [Replicate Swizzles](https://docs.microsoft.com/en-us/windows/desktop/direct3dhlsl/dx9-graphics-reference-asm-ps-registers-modifiers-source-register-swizzling#replicate-swizzle) (that is, they only accept input Source Parameters in the form of .xxxx, .yyyy, .zzzz, or .wwww). Of these instructions, there are six that D3D8 does not validate the [Replicate Swizzle](https://docs.microsoft.com/en-us/windows/desktop/direct3dhlsl/dx9-graphics-reference-asm-ps-registers-modifiers-source-register-swizzling#replicate-swizzle) for, but D3D9 does (and will fail validation if the replicate swizzle is not used on their sole Source Parameter). These instructions are:
* [exp](https://docs.microsoft.com/en-us/windows/desktop/direct3dhlsl/exp---vs)
* [expp](https://docs.microsoft.com/en-us/windows/desktop/direct3dhlsl/expp---vs)
* [log](https://docs.microsoft.com/en-us/windows/desktop/direct3dhlsl/log---vs)
* [logp](https://docs.microsoft.com/en-us/windows/desktop/direct3dhlsl/logp---vs)
* [rcp](https://docs.microsoft.com/en-us/windows/desktop/direct3dhlsl/rcp---vs)
* [rsq](https://docs.microsoft.com/en-us/windows/desktop/direct3dhlsl/rsq---vs)

To fix this issue, we modify the [Source Swizzle](https://docs.microsoft.com/en-us/windows/desktop/direct3dhlsl/dx9-graphics-reference-asm-ps-registers-modifiers-source-register-swizzling) on the [Source Parameter Token](https://docs.microsoft.com/en-us/windows-hardware/drivers/display/source-parameter-token) in the shader bytecode for these instructions (if the instruction does not already use a [Replicate Swizzle](https://docs.microsoft.com/en-us/windows/desktop/direct3dhlsl/dx9-graphics-reference-asm-ps-registers-modifiers-source-register-swizzling#replicate-swizzle)).

### Validation: Matrix Destination Write Masks

The D3D8 validator relaxes the [Write Mask](https://docs.microsoft.com/en-us/windows/desktop/direct3dhlsl/dx9-graphics-reference-asm-vs-registers-modifiers-masking#destination-register-masking-1) requirements on the matrix multiply (mAxB) instructions (D3D8 allows arbitrary write masks to all matrix instruction destination registers, whereas D3D9 has specific [Write Mask](https://docs.microsoft.com/en-us/windows/desktop/direct3dhlsl/dx9-graphics-reference-asm-vs-registers-modifiers-masking#destination-register-masking-1) requirements for each instruction), so we need to make sure that the instructions' [Write Masks](https://docs.microsoft.com/en-us/windows/desktop/direct3dhlsl/dx9-graphics-reference-asm-vs-registers-modifiers-masking#destination-register-masking-1) are set exactly to spec to the D3D9 requirements.

|Instruction|D3D9 Required Dst Write Mask|
|-----------|----------------------------|
|[m3x2](https://docs.microsoft.com/en-us/windows/desktop/direct3dhlsl/m3x2---vs)       |.xy                         |
|[m3x3](https://docs.microsoft.com/en-us/windows/desktop/direct3dhlsl/m3x3---vs)       |.xyz                        |
|[m3x4](https://docs.microsoft.com/en-us/windows/desktop/direct3dhlsl/m3x4---vs)       |.xyzw                       |
|[m4x3](https://docs.microsoft.com/en-us/windows/desktop/direct3dhlsl/m4x3---vs)       |.xyz                        |
|[m4x4](https://docs.microsoft.com/en-us/windows/desktop/direct3dhlsl/m4x4---vs)       |.xyzw                       |

It's simple enough to change the [Destination Parameter Token](https://docs.microsoft.com/en-us/windows-hardware/drivers/display/destination-parameter-token)'s [Write Mask](https://docs.microsoft.com/en-us/windows/desktop/direct3dhlsl/dx9-graphics-reference-asm-vs-registers-modifiers-masking#destination-register-masking-1) to have the proper [Write Mask](https://docs.microsoft.com/en-us/windows/desktop/direct3dhlsl/dx9-graphics-reference-asm-vs-registers-modifiers-masking#destination-register-masking-1) as per the above table. However, doing so introduces a new problem: In the case that we "shrink" a [Write Mask](https://docs.microsoft.com/en-us/windows/desktop/direct3dhlsl/dx9-graphics-reference-asm-vs-registers-modifiers-masking#destination-register-masking-1) (for example from .xyzw in D3D8 for a [m3x2](https://docs.microsoft.com/en-us/windows/desktop/direct3dhlsl/m3x2---vs) instruction to be the proper .xy in D3D9), it's now possible that we've caused some [Temporary (r#) Registers](https://docs.microsoft.com/en-us/windows/desktop/direct3dhlsl/dx9-graphics-reference-asm-vs-registers-temporary) or [Output (o#) Registers](https://docs.microsoft.com/en-us/windows/desktop/direct3dhlsl/dx9-graphics-reference-asm-vs-registers-output) to have uninitialized components (which would fail validation if these components later get read or written into an [Output (o#) Register](https://docs.microsoft.com/en-us/windows/desktop/direct3dhlsl/dx9-graphics-reference-asm-vs-registers-output)). So in order to work around this new issue, we detect this case and then assemble new instructions immediately after the offending (newly shrunk dest [Write Mask](https://docs.microsoft.com/en-us/windows/desktop/direct3dhlsl/dx9-graphics-reference-asm-vs-registers-modifiers-masking#destination-register-masking-1)) instruction. Specifically, we want to use a completely non-destructive instruction that dirties the register's components but doesn't require new constant registers or new temporary registers for storing and then restoring the value already present in that register. I ended up selecting this instruction:

`ADD rN.zwzw rN.xxxx -rN.xxxx`

It's an [ADD instruction](https://docs.microsoft.com/en-us/windows/desktop/direct3dhlsl/add---vs) with a self-negate (which is legal HLSL bytecode insofar as I can tell). This causes the register to be added to itself, and then subtracted from itself, which undoes the write but the D3D9 validator considers the register as properly "dirtied". Additionally this new instruction uses a .xxxx [Replicate Swizzle](https://docs.microsoft.com/en-us/windows/desktop/direct3dhlsl/dx9-graphics-reference-asm-ps-registers-modifiers-source-register-swizzling#replicate-swizzle), which should always be valid as all of the D3D9-approved matrix instruction [Write Masks](https://docs.microsoft.com/en-us/windows/desktop/direct3dhlsl/dx9-graphics-reference-asm-vs-registers-modifiers-masking#destination-register-masking-1) must write out the .xy components at a minimum. As a working example:

An original D3D8 vertex shader might contain the following instructions:
```
m3x2 r0, v1, c0 ; This is performing a 3x2 matrix multiplication between v1.xyz as a 3-vector
; and the matrix:
; [c0.x, c0.y, c0.z]
; [c1.x, c1.y, c1.z]
; The matrix shader instructions are special in that they have implied 
; consecutive source registers after the final source register listed
mov oPos0, r0
```

This becomes problematic when we modify the [m3x2 instruction](https://docs.microsoft.com/en-us/windows/desktop/direct3dhlsl/m3x2---vs) to have the proper .xy write-mask as required by D3D9 like this:
```
m3x2 r0.xy, v1, c0 ; This is performing a 3x2 matrix multiplication between v1.xyz as a 3-vector
; and the matrix:
; [c0.x, c0.y, c0.z]
; [c1.x, c1.y, c1.z]
; The matrix shader instructions are special in that they have implied 
; consecutive source registers after the final source register listed
mov oPos0, r0 ; Error: Now r0.zw are uninitialized!
```
This is problematic because it looks to the D3D9 shader validator like r0.zw is uninitialized, and it is illegal to read from uninitialized register channels. So to fix this, we assemble our new [ADD instruction](https://docs.microsoft.com/en-us/windows/desktop/direct3dhlsl/add---vs) immediately after the matrix instruction like so:
```
m3x2 r0.xy, v1, c0 ; This is performing a 3x2 matrix multiplication between v1.xyz as a 3-vector
; and the matrix:
; [c0.x, c0.y, c0.z]
; [c1.x, c1.y, c1.z]
; The matrix shader instructions are special in that they have implied 
; consecutive source registers after the final source register listed
add r0.zw, r0.x, -r0.x ; Newly assembled ADD instruction that does nothing but counts as 
; dirtying the register channels
mov oPos0, r0
```

### Immediate Constants

Unlike D3D9, which embeds shader Immediate Constants into its shader bytecode streams via [DEF Instructions](https://docs.microsoft.com/en-us/windows/desktop/direct3dhlsl/def---vs), D3D8 passes in Immediate Constant values as part of its D3D8 Vertex Declaration. You can read more about the structure and format of the D3D8 Vertex Declaration [here](https://github.com/code-tom-code/D3D8toD3D9shim/blob/master/d3d8tod3d9shim/originalD3D8/d3d8types.h#L610). Essentially while we're parsing the D3D8 Vertex Declaration, we scan the declaration bytestream looking for **D3DVSD_TOKEN_CONSTMEM**, which represent Immediate Constants. In order to convert vertex shaders with Immediate Constants in their D3D8 Vertex Declarations, we need to assemble some new [DEF Instructions](https://docs.microsoft.com/en-us/windows/desktop/direct3dhlsl/def---vs) at the beginning of our shader bytecode stream (inbetween the end of the [DCL Instruction](https://docs.microsoft.com/en-us/windows/desktop/direct3dhlsl/dcl-usage-input-register---vs) section but before the first real shader instruction).
