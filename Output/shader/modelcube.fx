//----------------------------------------------------------------
struct VertexInputType
{
	float4 position : POSITION;
	float4 color : COLOR;
	float2 texCoord0 : TEXCOORD0;
};
//----------------------------------------------------------------
struct VertexOutputType
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float2 texCoord0 : TEXCOORD0;
};
//----------------------------------------------------------------
//接收 VertexShader 所需要的变量
//----------------------------------------------------------------
cbuffer BufferMatrix
{
	matrix g_matWorldViewProj;
};
//----------------------------------------------------------------
//接收 PixelShader 所需要的变量
//----------------------------------------------------------------
Texture2D g_Texture1[2];
//----------------------------------------------------------------
SamplerState g_SampleType
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
	AddressW = WRAP;
	MipLODBias = 0.0f;
	MaxAnisotropy = 1;
	
	ComparisonFunc = ALWAYS;
	//BorderColor[0] = 0.0f;
	//BorderColor[1] = 0.0f;
	//BorderColor[2] = 0.0f;
	//BorderColor[3] = 0.0f;
	MinLOD = 0.0f;
	MaxLOD = (3.402823466e+38f); //D3D11_FLOAT32_MAX
};
//----------------------------------------------------------------
VertexOutputType ShaderVS(VertexInputType input)
{
	VertexOutputType output;

	// Change the position vector to be 4 units for proper matrix calculations.
	input.position.w = 1.0f;
	
	// Calculate the position of the vertex against the world, view, and projection matrices.
	output.position = mul(input.position, g_matWorldViewProj);
	
	output.color = input.color;
	
	// Store the texture coordinates for the pixel shader.
	output.texCoord0 = input.texCoord0;

	return output;
}
//----------------------------------------------------------------
float4 ShaderPS(VertexOutputType input) : SV_TARGET
{
	// Sample the pixel color from the texture using the sampler at this texture coordinate location.
	//float4 outputColor0 = g_Texture1[0].SampleLevel(g_SampleType, input.texCoord0, 0);
	float4 outputColor0 = g_Texture1[0].Sample(g_SampleType, input.texCoord0);
	if (input.color.a > 0.001f)
	{
		float fOldAlpha = outputColor0.a;
		float fSrcAlpha = 1.0f - input.color.a;
		outputColor0 = outputColor0 * fSrcAlpha + input.color * (1.0f - fSrcAlpha);
		outputColor0.a = fOldAlpha;
	}
	
	return outputColor0;
}
//----------------------------------------------------------------
technique11 ShaderTec
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, ShaderVS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, ShaderPS()));
	}
}
//----------------------------------------------------------------


