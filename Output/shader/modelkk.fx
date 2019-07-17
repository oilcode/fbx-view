//----------------------------------------------------------------
struct VertexInputType
{
	uint3 ValueIndex : POSITION0;
	uint4 BoneIndex : COLOR0;
	float4 BoneWeight : COLOR1;
};
//----------------------------------------------------------------
struct VertexOutputType
{
	float4 Position : SV_POSITION;
	float2 TexCoord : TEXCOORD;
	float4 PositionInWorld : COLOR0;
	float4 NormalInWorld : COLOR1;
};
//----------------------------------------------------------------
//接收 VertexShader 所需要的变量
//----------------------------------------------------------------
cbuffer BufferMatrix
{
	matrix g_matWorld;
	matrix g_matViewProj;
	matrix g_matWorldInvTranspose;
	uint g_VertexTextureWidth;
	uint g_VertexTexturePosCount;
	uint g_VertexTextureNormalCount;
	uint g_VertexTextureUVCount;
	uint g_AnimTextureWidth;
	uint g_AnimTextureBoneCount;
	uint g_AnimTextureKeyFrameIndex;
};
//----------------------------------------------------------------
//接收 PixelShader 所需要的变量
//----------------------------------------------------------------
//0号贴图是BaseTexture
//1号贴图是顶点结构体成员的值
//2号贴图是骨骼动画
Texture2D g_TextureList[3];
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
void CalculatePosNormalUV(in uint3 ValueIndex, out float4 thePos, out float4 theNormal, out float2 theUV)
{
	uint sizeofPixel = 4;
	uint sizeofPos = 12;
	uint sizeofNormal = 12;
	uint sizeofUV = 8;
	uint2 tempUV;
	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	uint PosPixelIndex = sizeofPos * ValueIndex[0] / sizeofPixel;
	tempUV[0] = PosPixelIndex % g_VertexTextureWidth;
	tempUV[1] = PosPixelIndex / g_VertexTextureWidth;
	thePos.x = g_TextureList[1][tempUV];
	//
	PosPixelIndex += 1;
	tempUV[0] = PosPixelIndex % g_VertexTextureWidth;
	tempUV[1] = PosPixelIndex / g_VertexTextureWidth;
	thePos.y = g_TextureList[1][tempUV];
	//
	PosPixelIndex += 1;
	tempUV[0] = PosPixelIndex % g_VertexTextureWidth;
	tempUV[1] = PosPixelIndex / g_VertexTextureWidth;
	thePos.z = g_TextureList[1][tempUV];
	thePos.w = 1.0f;
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	
	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	uint NormalPixelIndex = (sizeofPos * g_VertexTexturePosCount + sizeofNormal * ValueIndex[1])/ sizeofPixel;
	tempUV[0] = NormalPixelIndex % g_VertexTextureWidth;
	tempUV[1] = NormalPixelIndex / g_VertexTextureWidth;
	theNormal.x = g_TextureList[1][tempUV];
	//
	NormalPixelIndex += 1;
	tempUV[0] = NormalPixelIndex % g_VertexTextureWidth;
	tempUV[1] = NormalPixelIndex / g_VertexTextureWidth;
	theNormal.y = g_TextureList[1][tempUV];
	//
	NormalPixelIndex += 1;
	tempUV[0] = NormalPixelIndex % g_VertexTextureWidth;
	tempUV[1] = NormalPixelIndex / g_VertexTextureWidth;
	theNormal.z = g_TextureList[1][tempUV];
	theNormal.w = 0.0f;
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	uint UVPixelIndex = (sizeofPos * g_VertexTexturePosCount + sizeofNormal * g_VertexTextureNormalCount + sizeofUV * ValueIndex[2])/ sizeofPixel;
	tempUV[0] = UVPixelIndex % g_VertexTextureWidth;
	tempUV[1] = UVPixelIndex / g_VertexTextureWidth;
	theUV.x = g_TextureList[1][tempUV];
	//
	UVPixelIndex += 1;
	tempUV[0] = UVPixelIndex % g_VertexTextureWidth;
	tempUV[1] = UVPixelIndex / g_VertexTextureWidth;
	theUV.y = g_TextureList[1][tempUV];
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
}
//----------------------------------------------------------------
float4x4 GetSingleBoneAnimMatrix(in uint BoneIndex)
{
	float4x4 finalMat;
	uint sizeofPixel = 4;
	uint ElementSize = 48;
	uint startPixelIndex = (g_AnimTextureKeyFrameIndex * g_AnimTextureBoneCount * ElementSize + BoneIndex * ElementSize) / sizeofPixel;
	uint2 tempUV;
	tempUV[0] = startPixelIndex % g_AnimTextureWidth;
	tempUV[1] = startPixelIndex / g_AnimTextureWidth;
	finalMat._m00 = g_TextureList[2][tempUV];
	//
	startPixelIndex += 1;
	tempUV[0] = startPixelIndex % g_AnimTextureWidth;
	tempUV[1] = startPixelIndex / g_AnimTextureWidth;
	finalMat._m01 = g_TextureList[2][tempUV];
	//
	startPixelIndex += 1;
	tempUV[0] = startPixelIndex % g_AnimTextureWidth;
	tempUV[1] = startPixelIndex / g_AnimTextureWidth;
	finalMat._m02 = g_TextureList[2][tempUV];
	//
	startPixelIndex += 1;
	tempUV[0] = startPixelIndex % g_AnimTextureWidth;
	tempUV[1] = startPixelIndex / g_AnimTextureWidth;
	finalMat._m10 = g_TextureList[2][tempUV];
	//
	startPixelIndex += 1;
	tempUV[0] = startPixelIndex % g_AnimTextureWidth;
	tempUV[1] = startPixelIndex / g_AnimTextureWidth;
	finalMat._m11 = g_TextureList[2][tempUV];
	//
	startPixelIndex += 1;
	tempUV[0] = startPixelIndex % g_AnimTextureWidth;
	tempUV[1] = startPixelIndex / g_AnimTextureWidth;
	finalMat._m12 = g_TextureList[2][tempUV];
	//
	startPixelIndex += 1;
	tempUV[0] = startPixelIndex % g_AnimTextureWidth;
	tempUV[1] = startPixelIndex / g_AnimTextureWidth;
	finalMat._m20 = g_TextureList[2][tempUV];
	//
	startPixelIndex += 1;
	tempUV[0] = startPixelIndex % g_AnimTextureWidth;
	tempUV[1] = startPixelIndex / g_AnimTextureWidth;
	finalMat._m21 = g_TextureList[2][tempUV];
	//
	startPixelIndex += 1;
	tempUV[0] = startPixelIndex % g_AnimTextureWidth;
	tempUV[1] = startPixelIndex / g_AnimTextureWidth;
	finalMat._m22 = g_TextureList[2][tempUV];
	//
	startPixelIndex += 1;
	tempUV[0] = startPixelIndex % g_AnimTextureWidth;
	tempUV[1] = startPixelIndex / g_AnimTextureWidth;
	finalMat._m30 = g_TextureList[2][tempUV];
	//
	startPixelIndex += 1;
	tempUV[0] = startPixelIndex % g_AnimTextureWidth;
	tempUV[1] = startPixelIndex / g_AnimTextureWidth;
	finalMat._m31 = g_TextureList[2][tempUV];
	//
	startPixelIndex += 1;
	tempUV[0] = startPixelIndex % g_AnimTextureWidth;
	tempUV[1] = startPixelIndex / g_AnimTextureWidth;
	finalMat._m32 = g_TextureList[2][tempUV];
	//
	finalMat._m03 = 0.0f;
	finalMat._m13 = 0.0f;
	finalMat._m23 = 0.0f;
	finalMat._m33 = 1.0f;
	//
	return finalMat;
}
//----------------------------------------------------------------
void CalculateBoneAnim(in uint4 BoneIndex, in float4 BoneWeight, inout float4 thePos, inout float4 theNormal)
{
	float4 finalPos = {0.0f, 0.0f, 0.0f, 0.0f};
	float4 finalNormal = {0.0f, 0.0f, 0.0f, 0.0f};
	for (uint i = 0; i < 4; ++i)
	{
		if (BoneWeight[i] > 0.001f)
		{
			float4x4 BoneMat = GetSingleBoneAnimMatrix(BoneIndex[i]);
			//
			finalPos += mul(thePos, BoneMat) * BoneWeight[i];
			//
			finalNormal += mul(theNormal, BoneMat) * BoneWeight[i];
		}
		else
		{
			break;
		}
	}
	finalPos.w = 1.0f;
	finalNormal.w = 0.0f;
	thePos = finalPos;
	theNormal = finalNormal;
}
//----------------------------------------------------------------
float4 CalculatePointLight(in float4 thePos, in float4 theNormal)
{
	float3 PointLightColor = {1.0f, 0.0f, 0.0f};
	float3 PointLightPos = {0.0f, 0.0f, 3.0f};
	float PointLightRange = 90.0f;
	float PointLightBright = 0.5f;
	
	float3 Direction = float3(PointLightPos.x-thePos.x, PointLightPos.y-thePos.y, PointLightPos.z-thePos.z);
	float Distance = length(Direction);
	Direction = normalize(Direction);
	float fDot = dot(Direction, float3(theNormal.xyz));
	
	float4 PointLightDiffuse = {0.0f, 0.0f, 0.0f, 0.0f};
	if (Distance < PointLightRange && fDot > 0.01f)
	{
		float scale =  (PointLightRange - Distance) / PointLightRange;
		PointLightDiffuse.xyz = PointLightColor * scale;
		PointLightDiffuse.w = PointLightBright * scale;
	}
	return PointLightDiffuse;
}
//----------------------------------------------------------------
VertexOutputType ShaderVS(VertexInputType input)
{
	float4 thePos;
	float4 theNormal;
	float2 theUV;
	CalculatePosNormalUV(input.ValueIndex, thePos, theNormal, theUV);
	
	//计算骨骼动画
	//动画帧序号为-1，表示无效值，不计算骨骼动画
	if (g_AnimTextureKeyFrameIndex != 0xFFFFFFFF)
	{
		CalculateBoneAnim(input.BoneIndex, input.BoneWeight, thePos, theNormal);
	}
	
	//计算世界空间内的坐标和法线
	thePos = mul(thePos, g_matWorld);
	theNormal = mul(theNormal, g_matWorldInvTranspose);
	
	VertexOutputType output;
	output.Position = mul(thePos, g_matViewProj);
	output.TexCoord = theUV;
	output.PositionInWorld = thePos;
	output.NormalInWorld = theNormal;
	return output;
}
//----------------------------------------------------------------
float4 ShaderPS(VertexOutputType input) : SV_TARGET
{
	//计算点光源
	//float4 PointLightDiffuse = CalculatePointLight(input.PositionInWorld, input.NormalInWorld);
	
	float4 outputColor = g_TextureList[0].Sample(g_SampleType, input.TexCoord);
	
	//if (PointLightDiffuse.w > 0.01f)
	//{
	//	outputColor = outputColor * (1.0f-PointLightDiffuse.w) + PointLightDiffuse * PointLightDiffuse.w;
	//	outputColor.w = 1.0f;
	//}
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


