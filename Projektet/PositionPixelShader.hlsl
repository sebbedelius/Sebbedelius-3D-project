struct VS_OUT
{
	float4 pos;
};

float4 PPS_main() : SV_TARGET
{

	return float4(1.0f, 0.0f, 0.0f, 1.0f);
}