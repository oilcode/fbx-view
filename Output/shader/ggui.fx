//----------------------------------------------------------------
struct VertexInputType
{
	float4 position : POSITION;
	float3 texCoord : TEXCOORD;
	float4 color    : COLOR;
};
//----------------------------------------------------------------
struct VertexOutputType
{
	float4 position : SV_POSITION;
	float3 texCoord : TEXCOORD;
	float4 color    : COLOR;
};
//----------------------------------------------------------------
//接收 VertexShader 所需要的变量
//----------------------------------------------------------------
cbuffer BufferMatrix
{
	matrix g_MatrixViewProj;
};
//----------------------------------------------------------------
//接收 PixelShader 所需要的变量
//----------------------------------------------------------------
Texture2D g_TextureList[4];
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
//BlendState g_BlendType
//{
	//AlphaToCoverageEnable = FALSE;
	////IndependentBlendEnable = FALSE;
	////RenderTarget[0].BlendEnable = TRUE;
	//BlendEnable[0] = TRUE;
//};
//----------------------------------------------------------------
VertexOutputType ShaderVS(VertexInputType input)
{
	input.position.w = 1.0f;
	
	VertexOutputType output;
	output.position = mul(input.position, g_MatrixViewProj);
	output.texCoord = input.texCoord;
	output.color = input.color;
	
	return output;
}
//----------------------------------------------------------------
float4 ShaderPS(VertexOutputType input) : SV_TARGET
{
	float2 theTexCoord = float2(input.texCoord.x, input.texCoord.y);
	int nTextureIndex = input.texCoord.z;
	float4 outputColor;
	
	if (nTextureIndex == 0)
	{
		outputColor = g_TextureList[0].Sample(g_SampleType, theTexCoord);
	}
	else if (nTextureIndex == 1)
	{
		outputColor = g_TextureList[1].Sample(g_SampleType, theTexCoord);
	}
	else if (nTextureIndex == 2)
	{
		outputColor = g_TextureList[2].Sample(g_SampleType, theTexCoord);
	}
	else if (nTextureIndex == 3)
	{
		outputColor = g_TextureList[3].Sample(g_SampleType, theTexCoord);
	}
	
	if (input.color.a > 0.0f)
	{
		float fOldAlpha = outputColor.a;
		float fSrcAlpha = 1.0f - input.color.a;
		outputColor = outputColor * fSrcAlpha + input.color * (1.0f - fSrcAlpha);
		outputColor.a = fOldAlpha;
	}
	
	return outputColor;
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


