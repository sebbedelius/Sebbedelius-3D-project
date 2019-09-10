cbuffer SHADOW_GS_CONSTANT_BUFFER : register(b0)
{
	matrix theWorld;
	matrix theWorldViewProj;
	matrix theSunWorldViewProj;
	matrix theView;
};
struct VS_OUT
{
	float3 Pos : POSITION;
	float3 Color : COLOR;
};
struct GSOutput
{
	float4 Pos : SV_POSITION;
	float4 PosSun : TEXCOORD1;
	float3 Color : COLOR;
	float4 WorldPos : W_POSITION;
	float4 FaceNormal : NORMAL;	
};

[maxvertexcount(3)]
void CGS_main(
	triangle VS_OUT input[3],
	inout TriangleStream< GSOutput > output
)
{

	float3 faceEdgeA = input[1].Pos - input[0].Pos;
	float3 faceEdgeB = input[2].Pos - input[0].Pos;
	float3 faceNormal = normalize(cross(faceEdgeA, faceEdgeB));

	for (uint i = 0; i < 3; i++)
	{
		GSOutput element = (GSOutput)0;

		element.Pos = mul(float4(input[i].Pos, 1.0f), theWorldViewProj);
		element.Color = input[i].Color;
		element.WorldPos = mul(float4(input[i].Pos, 1.0f), theWorld);
		element.FaceNormal = mul(float4(faceNormal, 0.0f), theWorld);
		element.PosSun = mul(float4(input[i].Pos, 1.0f), theSunWorldViewProj);		 

		output.Append(element);

	}
}