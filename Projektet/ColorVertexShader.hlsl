struct VS_IN
{
	float3 Pos : POSITION;
	float3 Color : COLOR;
};

struct VS_OUT
{
	float3 Pos : POSITION;
	float3 Color : COLOR;
};
//-----------------------------------------------------------------------------------------
// VertexShader: VSScene
//-----------------------------------------------------------------------------------------
VS_OUT CVS_main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;

	output.Pos = input.Pos;
	output.Color = input.Color;

	return output;
}