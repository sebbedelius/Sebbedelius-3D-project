Texture2D txShadowMap : register(t0);
SamplerState sampAni :register(s0);
SamplerComparisonState samShadow : register(s1);

static const float SUN_SIZE = 2048.0f;
static const float SUN_DX = 1.0f / SUN_SIZE;

struct GS_OUT
{
	float4 Pos : SV_POSITION;
	float4 PosSun : TEXCOORD1;
	float3 Color : COLOR;
	float4 WorldPos : W_POSITION;
	float4 FaceNormal : NORMAL;	
};

float PCF(Texture2D shadowMap,
	float2 projTexCoord, SamplerState sampAni,
	float depth)
{
	float shadowFactor = 0.0f;	

	float s0 = shadowMap.Sample(sampAni, projTexCoord).r;
	float s1 = shadowMap.Sample(sampAni, projTexCoord + float2(SUN_DX, 0)).r;
	float s2 = shadowMap.Sample(sampAni, projTexCoord + float2(0, SUN_DX)).r;
	float s3 = shadowMap.Sample(sampAni, projTexCoord + float2(SUN_DX, SUN_DX)).r;

	//Is the depth of the pixel we are looking at in shadow?	
	float r0 = depth <= s0;
	float r1 = depth <= s1;
	float r2 = depth <= s2;
	float r3 = depth <= s3;

	float2 texelPos = SUN_SIZE * projTexCoord;

	float2 t = frac(texelPos);

	//Interpolate the different results to determine the shadow factor
	shadowFactor = lerp(lerp(r0, r1, t.x),
		lerp(r2, r3, t.x), t.y);

	if (shadowFactor < 0.4)
	{
		shadowFactor = 0.5;
	}

	return shadowFactor;
}

float CalculateShadowFactor(SamplerState sampAni,
	Texture2D shadowMap, float2 projTexCoord, float depth)
{
	float color = 0.0f;
	float dx = 1.0f / SUN_SIZE;

	float2 offsets[9] =
	{
		float2(-dx, -dx), float2(0.0f, -dx), float2(dx, -dx),
		float2(-dx, 0.0f), float2(0.0f, 0.0f), float2(dx, 0.0f),
		float2(-dx, +dx), float2(0.0f, +dx), float2(dx, +dx)
	};

	for (int i = 0; i < 9; i++)
	{
		color += PCF(shadowMap, projTexCoord + offsets[i], sampAni, depth);
	}

	color /= 9.0f;

	return color;
}

float4 CPS_main(GS_OUT input) : SV_TARGET
{
	float3 color = input.Color;
	float2 projTexCoord;
	float shadowFactor;

	//Transform from light's NDC space to texture space
	projTexCoord.x = input.PosSun.x / input.PosSun.w / 2.0f + 0.5f;
	projTexCoord.y = -input.PosSun.y / input.PosSun.w / 2.0f + 0.5f;

	float dp = input.PosSun.z / input.PosSun.w; //Depth from the sun to the position		

	shadowFactor = CalculateShadowFactor(sampAni, txShadowMap, projTexCoord, dp);

	color *= shadowFactor;

	return float4(color, 1.0f);	
}