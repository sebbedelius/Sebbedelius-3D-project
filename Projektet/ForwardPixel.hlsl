Texture2D txDiffuse : register(t0); 
Texture2D txBlend : register(t1);
Texture2D txShadowMap : register(t2);
SamplerState sampAni :register(s0);
SamplerState sampBorder : register(s1);

cbuffer CAMERA_BUFFER
{
	float4 CamPos;
};
float3 CalcPhongShading(in float3 position, in float3 normal, in float3 diffuseAlbedo,
	in float3 specularAlbedo, in float specularPower)
{
	float angle = 0;
	float3 u;
	float3 normalPrim;
	float3 lightDir;
	float3 viewDir;
	float3 reflection;
	float Id; // Diffuse intensity
	float Is; // Specular intensity
	float3 intensity;
	float3 lighting = (0.0f, 0.0f, 0.0f);

	// Calculate the angle between the normal and the light vector
	lightDir = float3(50.0f, 50.0f, 50.0f) - position; //From the surface to the light	
	angle = acos((max(dot(lightDir, normal) / (length(lightDir) * length(normal)), 0.0f)));

	//Do the light calculations
	if (length(normal) >= 0 && angle != 0)
	{
		normalPrim = normalize(normal);
		lightDir = normalize(lightDir);
		viewDir = normalize(CamPos.xyz - position); //From the surface to the camera
													//Calculate reflection vector
		u = normalPrim - lightDir;
		reflection = lightDir + 2 * u;

		Id = diffuseAlbedo * cos(angle);
		Is = specularAlbedo * pow(max(dot(reflection, viewDir), 0.0f), specularPower);

		intensity = float3(1.0f, 1.0f, 1.0f) + (Id + Is) * float3(1.0f, 1.0f, 1.0f);

		lighting = intensity * diffuseAlbedo;
	}

	return lighting;
}

struct GS_OUT
{
	float4 Pos : SV_POSITION;	
	float2 Tex : TEXCOORD;
	float4 PosSun : TEXCOORD1;
	float4 WorldPos : W_POSITION;
	float4 FaceNormal : NORMAL;
};

float4 PST_main(GS_OUT input) : SV_TARGET
{
	float3 s = txDiffuse.Sample(sampAni, input.Tex).xyz;

	s = CalcPhongShading(input.WorldPos.xyz, input.FaceNormal.xyz, s, float3(0.5f, 0.5f, 0.5f), 2.0f);
	
	float2 projTexCoord;

	//Transform from light's NDC space to texture space
	projTexCoord.x = input.PosSun.x / input.PosSun.w / 2.0f + 0.5f;
	projTexCoord.y = -input.PosSun.y / input.PosSun.w / 2.0f + 0.5f;

	float dp = input.PosSun.z / input.PosSun.w; //Depth from the sun to the position
	float sp = txShadowMap.Sample(sampAni, projTexCoord).r; //Depth from the sun to the nearest object	

	if (sp < dp)
	{
		s = float3(0.5f, 0.5f, 0.5f) * s;
	}

	return float4(s, 1.0f);
}