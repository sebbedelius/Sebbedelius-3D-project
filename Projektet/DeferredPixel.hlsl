Texture2D txDiffuse : register(t0);
Texture2D txBlend : register(t1);
Texture2D txShadowMap : register(t2);
SamplerState sampAni : register(s0);
SamplerState sampBorder : register(s1);

cbuffer PS_CONSTANT_BUFFER
{
	float3 specularAlbedo;
	float specularPower;
};

struct GS_OUT
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;
	float4 PosSun : TEXCOORD1;
	float4 WorldPos : W_POSITION;
	float4 FaceNormal : NORMAL;
};

struct PS_OUT
{
	float4 Normal : SV_Target0;
	float4 DiffuseAlbedo : SV_Target1;
	float4 SpecularAlbedo : SV_Target2;
	float4 Position : SV_Target3;

};
PS_OUT PS_main(GS_OUT input) : SV_Target
{	
	PS_OUT output;
	
	float4 diffuseAlbedo = txDiffuse.Sample(sampAni, input.Tex);
	float4 diffuseBlend = txBlend.Sample(sampBorder, input.Tex);

	float3 normal = normalize(input.FaceNormal.xyz);

	output.Normal = float4(normal, 0.0f);
	output.DiffuseAlbedo = diffuseAlbedo - diffuseBlend;	
	output.SpecularAlbedo = float4(specularAlbedo, specularPower);
	output.Position = input.WorldPos;

	return output;	
};