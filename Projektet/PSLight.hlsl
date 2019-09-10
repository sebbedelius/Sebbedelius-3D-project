Texture2D NormalTexture :register(t0);
Texture2D DiffuseAlbedoTexture : register(t1);
Texture2D SpecularAlbedoTexture : register(t2);
Texture2D PositionTexture : register(t3);
SamplerState SampleTypePoint : register(s0);

cbuffer LightParams : register(b0)
{
	float4 LightPos;
	float4 LightColor;		
	float4 LightAmbient; 
};

cbuffer CameraPos : register(b1)
{
	float4 CamPos;
};

void GetGBufferAttributes(in float2 tex, out float4 normal, out float4 position,
	out float4 diffuseAlbedo, out float3 specularAlbedo,
	out float specularPower)
{
	normal = NormalTexture.Sample(SampleTypePoint, tex);
	position = PositionTexture.Sample(SampleTypePoint, tex);
	diffuseAlbedo = DiffuseAlbedoTexture.Sample(SampleTypePoint, tex);
	float4 specular = SpecularAlbedoTexture.Sample(SampleTypePoint, tex);

	specularAlbedo = specular.xyz;
	specularPower = specular.w;
}

float3 CalcPhongShading(in float3 position, in float3 normal, in float3 diffuseAlbedo,
	in float3 specularAlbedo, in float specularPower)
{
	float cosAngle = 0;
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
	lightDir = LightPos.xyz - position; //From the surface to the light	
	cosAngle = max(dot(normalize(normal), normalize(lightDir)), 0.0f);
	
	//Do the light calculations
	if (length(normal) >= 0 && cosAngle >= 0)
	{
		normalPrim = normalize(normal);
		lightDir = normalize(lightDir);
		viewDir = normalize(CamPos.xyz - position); //From the surface to the camera

		//Calculate reflection vector
		u = normalPrim - lightDir;
		reflection = lightDir + 2 * u;

		Id = diffuseAlbedo * cosAngle;
		Is = specularAlbedo * LightColor.xyz * pow(max(dot(reflection, viewDir), 0.0f), specularPower);
		
		intensity = LightAmbient.xyz + (Id + Is);

		lighting = intensity *diffuseAlbedo;
	}	
	
	return lighting;
}

struct PSL_IN
{
	float4 Position : SV_POSITION;	
	float2 Tex : TEXCOORD;
};

float4 PSL_main(in PSL_IN input) : SV_Target
{
	float4 normal;
	float4 position;
	float4 diffuseAlbedo;
	float3 specularAlbedo;
	float specularPower;

	GetGBufferAttributes(input.Tex, normal, position,
		diffuseAlbedo, specularAlbedo, specularPower);

	float3 lighting = CalcPhongShading(position.xyz, normal.xyz, diffuseAlbedo.xyz,
		specularAlbedo, specularPower);

	return float4(lighting, 1.0f);		
}