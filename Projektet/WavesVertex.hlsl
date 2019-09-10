cbuffer VS_WAVES_BUFFER
{
	matrix theWaterTexTransform;
};

struct VS_IN
{
	float3 Pos : POSITION;
	float2 Tex: TEXCOORD;
};

struct VS_OUT
{
	float3 Pos : POSITION;
	float2 Tex: TEXCOORD;
};
//-----------------------------------------------------------------------------------------
// VertexShader: VSScene
//-----------------------------------------------------------------------------------------
VS_OUT VSW_main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;

	output.Pos = input.Pos;	
	output.Tex = mul(float4(input.Tex, 0.0f, 1.0f), theWaterTexTransform).xy;

	return output;
}