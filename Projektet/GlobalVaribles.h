#ifndef GLOBALVARIBLES_H
#define GLOBALVARIBLES_H
#include <d3d11.h>
#include "DirectXMath.h"
using namespace DirectX;

static const int WINDOW_WIDTH = 1280;
static const int WINDOW_HEIGHT = 720;

enum Rendering { Forward, Deferred };

const int NR_OF_TARGET_AND_RESOURCE_VIEWS = 4;
const int PI = 3.1415;

static const int SUN_SIZE = 2048;

enum ShaderType {
	Position, PositionColor, PositionTexture, PositionTextureForWaves, PositionTextureForDeferred, PositionTextureForLightPass,
	PositionColorWithIndexBuffer, PositionColorWithIndexBufferForDeferred,
	PositionTexWithIndexBuffer, PositionTexShadowPass, PositionColorShadowPass
};

struct Vertex
{
	DirectX::XMFLOAT3 pos;
};

struct Texture
{
	float u, v;
};

struct Normal
{
	float x, y, z;
};

struct TriangleVertexColor
{
	float x, y, z;
	float r, g, b;
};

struct TriangleVertexTex
{
	float x, y, z;
	float u, v;
};

struct TriangleVertexTexXM
{
	XMFLOAT3 pos;
	float u, v;
};

struct GS_CONSTANT_BUFFER
{
	XMMATRIX theWorld;
	XMMATRIX theWorldViewProj;
};

struct PS_CONSTANT_BUFFER
{
	XMFLOAT3 specularAlbedo;
	float specularPower;
};

struct SHADOW_GS_CONSTANT_BUFFER
{
	XMMATRIX theWorld;
	XMMATRIX theWorldViewProj;
	XMMATRIX theSunWorldViewProj;
	XMMATRIX theView;
};

struct SHADOW_VS_CONSTANT_BUFFER
{
	XMMATRIX theWorld;
	XMMATRIX theWorldViewProj;
};

struct PSL_LIGHT_BUFFER
{
	XMVECTOR theLightPos;
	XMVECTOR theLightColor;
	XMVECTOR theLightAmbient;
};

struct CAMERA_BUFFER
{
	XMVECTOR camPos;
};

struct VS_WAVES_BUFFER
{
	XMMATRIX theWaterTexTransform;
};

#endif
