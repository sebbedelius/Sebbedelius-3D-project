cbuffer VSL_CONSTANT_BUFFER : register(b0)
{
	matrix theWorld;
	matrix theWorldViewProj;
}; 
struct VSL_IN
{	
	float3 Position : POSITION;	
	float2 Tex : TEXCOORD;
};

struct PSL_IN
{
	float4 screenPos : SV_POSITION;	
	float2 Tex : TEXCOORD;
};

PSL_IN VSL_main(VSL_IN pos) 
{
	PSL_IN output;

	output.screenPos = float4(pos.Position, 1.0f);
	output.Tex = pos.Tex;

	return output;
}