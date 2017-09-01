//----------------------------------------------------------------
struct VertexInputType
{
	float4 position : POSITION;
	float4 normal : NORMAL;
	float4 color : COLOR;
	float2 texCoord0 : TEXCOORD0;
	float2 texCoord1 : TEXCOORD1;
};
//----------------------------------------------------------------
struct VertexOutputType
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float2 texCoord0 : TEXCOORD0;
	float2 texCoord1 : TEXCOORD1;
};
//----------------------------------------------------------------
//接收 VertexShader 所需要的变量
//----------------------------------------------------------------
cbuffer BufferMatrix
{
	float4x4 g_matWorldViewProj;
	float3 g_CameraPos;
	float3 g_CameraLookDir;
	float3 g_AmbientLightColor;
	float3 g_DirectionalLightColor;
	float3 g_DirectionalLightDir;
	float3 g_MaterialAmbientColor;
	float3 g_MaterialDiffuseColor;
	float4 g_MaterialSpecularColor; //前3个成员是颜色，第4个成员是光泽度
};
//----------------------------------------------------------------
//接收 PixelShader 所需要的变量
//----------------------------------------------------------------
Texture2D g_Texture1;
Texture2D g_Texture2;
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
void CalculateAmbientDirectionalLight(float3 vNormal, out float3 vDiffuse, out float3 vSpecular)
{
	vDiffuse = g_AmbientLightColor * g_MaterialAmbientColor;
	float fCos = dot(vNormal, g_DirectionalLightDir);
	if (fCos > 0.0f)
	{
		vDiffuse += g_DirectionalLightColor * g_MaterialDiffuseColor * fCos;
		vSpecular = g_DirectionalLightColor * g_MaterialSpecularColor.rgb * pow(fCos, g_MaterialSpecularColor.a);
	}
}
//----------------------------------------------------------------
VertexOutputType ShaderVS(VertexInputType input)
{
	VertexOutputType output;

	// Change the position vector to be 4 units for proper matrix calculations.
	input.position.w = 1.0f;
	
	float3 Vertex2Camera = normalize(g_CameraPos - input.position.xyz);
	float fCos = saturate(dot(Vertex2Camera, input.normal.xyz));
	
	// Calculate the position of the vertex against the world, view, and projection matrices.
	output.position = mul(input.position, g_matWorldViewProj);
	
	// Store the texture coordinates for the pixel shader.
	output.texCoord0 = input.texCoord0;
	//output.texCoord1 = input.texCoord1;
	output.texCoord1.x = fCos;
	output.texCoord1.y = fCos;
	output.color = input.color;

	return output;
}
//----------------------------------------------------------------
float4 ShaderPS(VertexOutputType input) : SV_TARGET
{
	// Sample the pixel color from the texture using the sampler at this texture coordinate location.
	float4 outputColor0 = g_Texture1.Sample(g_SampleType, input.texCoord0);
	
	float4 color1 = g_Texture2.Sample(g_SampleType, input.texCoord1);
	float theAlpha = input.texCoord1.x;
	outputColor0 = outputColor0 * (1.0 - theAlpha) + color1 * theAlpha;
	
	if (input.color.a > 0.0f)
	{
		float alpha = input.color.a;
		outputColor0 = outputColor0 * (1.0 - alpha) + input.color * alpha;
		outputColor0.a = 1.0f;
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
