cbuffer VS_SHADOW_CONSTANT_BUFFER
{
	matrix world;
	matrix worldViewProj;
};

struct VS_IN
{
	float3 Pos : POSITION;
	float3 Color : COLOR;
};

struct VS_OUT
{
	float4 Pos : SV_POSITION;	
};

VS_OUT CVSS_main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;

	output.Pos = mul(float4(input.Pos, 1.0f), worldViewProj);	

	return output;
}