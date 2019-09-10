cbuffer GS_CONSTANT_BUFFER //Just use the same as the geometry buffer
{
	matrix theWorld;
	matrix theWorldViewProj;
};

struct VS_IN
{
	float3 pos : POSITION;
};

struct VS_OUT
{
	float4 pos : SV_POSITION;
};

VS_OUT PVS_main( VS_IN input )
{
	VS_OUT output = (VS_OUT)0;

	output.pos = mul(float4(input.pos, 1.0f), theWorldViewProj);
	//output.pos = float4(input.pos, 1.0f);

	return output;
}