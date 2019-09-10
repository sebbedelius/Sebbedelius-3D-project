#ifndef SUN_H
#define SUN_H
#include <d3d11.h>
#include "DirectXMath.h"
#include "GlobalVaribles.h"
using namespace DirectX;

struct BoundingSphere
{
	XMFLOAT3 center;
	float radius;
};

//Used for the shadow map, so it's built like a static orthographic camera
class Sun
{
private:
	XMFLOAT4X4 mView;
	XMFLOAT4X4 mProj;	

	XMFLOAT3 mPosition;
	XMFLOAT3 mLook;
	XMFLOAT3 mUp;
	XMFLOAT3 mRight;	

	float mNearZ, mFarZ;		

public:
	Sun(UINT width = SUN_SIZE, UINT height = SUN_SIZE);
	~Sun();

	void setLens(UINT width, UINT height, float zn, float zf);	

	XMFLOAT4X4 getView() const;
	XMMATRIX getViewXM() const;
	XMFLOAT4X4 getProj() const;
	XMMATRIX getProjXM() const;		
};

#endif
