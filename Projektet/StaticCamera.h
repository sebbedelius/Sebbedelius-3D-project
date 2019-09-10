#ifndef STATICCAMERA_H
#define STATICCAMERA_H
#include <d3d11.h>
#include <DirectXMath.h>
#include "GlobalVaribles.h"
using namespace DirectX;

class StaticCamera
{
private:
	XMFLOAT3 mPosition;	
	XMFLOAT3 mUp;
	XMFLOAT3 mLook;	

	float mNearZ;
	float mFarZ;
	float mAspectRatio;
	float mFovY;	

	XMFLOAT4X4 mView;
	XMFLOAT4X4 mProj;

	void setLens();

public:
	StaticCamera();
	~StaticCamera();

	XMMATRIX getView() const;
	XMMATRIX getProj() const;

};


#endif

