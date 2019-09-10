cbuffer SHADOW_GS_CONSTANT_BUFFER : register(b0)
{
	matrix theWorld;
	matrix theWorldViewProj;
	matrix theSunWorldViewProj;
	matrix theView;
};

cbuffer CAMERA_BUFFER : register(b1)
{
	float4 camPos;
}

struct VS_OUT
{
	float3 Pos : POSITION;	
	float2 Tex : TEXCOORD;
};
struct GSOutput
{
	float4 Pos : SV_POSITION;		
	float2 Tex : TEXCOORD;
	float4 PosSun: TEXCOORD1;
	float4 WorldPos : W_POSITION;
	float4 FaceNormal : NORMAL;
};

[maxvertexcount(3)]
void GS_main(
	triangle VS_OUT input[3],
	inout TriangleStream< GSOutput > output
)
{	
	float3 faceEdgeA = mul(float4(input[1].Pos, 1.0f), theWorld).xyz - mul(float4(input[0].Pos, 1.0f), theWorld).xyz;	
	float3 faceEdgeB = mul(float4(input[2].Pos, 1.0f), theWorld).xyz - mul(float4(input[0].Pos, 1.0f), theWorld).xyz;
	float3 faceNormal = normalize(cross(faceEdgeA, faceEdgeB));

	float3 worldVertexPos = mul(float4(input[0].Pos, 1.0f), theWorld).xyz;

	if (dot((worldVertexPos - camPos), faceNormal) < 0)	//Back face culling
	{
		for (uint i = 0; i < 3; i++)
		{
			GSOutput element = (GSOutput)0;

			element.Pos = mul(float4(input[i].Pos, 1.0f), theWorldViewProj);
			element.Tex = input[i].Tex;
			element.PosSun = mul(float4(input[i].Pos, 1.0f), theSunWorldViewProj);
			element.WorldPos = mul(float4(input[i].Pos, 1.0f), theWorld);
			element.FaceNormal = mul(float4(faceNormal, 0.0f), theWorld);

			output.Append(element);

		}
	}	
}