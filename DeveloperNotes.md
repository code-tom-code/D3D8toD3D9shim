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



