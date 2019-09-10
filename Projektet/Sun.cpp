#include "Sun.h"

Sun::Sun(UINT width, UINT height)
{
	this->mPosition = XMFLOAT3(15.0f, 15.0f, 0.0f);
	this->mRight = XMFLOAT3(1.0f, 0.0f, 0.0f);
	this->mUp = XMFLOAT3(0.0f, 1.0f, 0.0f);
	this->mLook = XMFLOAT3(-15.0f, 0.0f, 0.1f);

	this->setLens(width, height, 0.1f, 50.0f);	
}

Sun::~Sun()
{
}

void Sun::setLens(UINT width, UINT height, float zn, float zf)
{
	BoundingSphere bSphere;
	bSphere.center = XMFLOAT3(0.0f, 0.0f, 0.0f);
	bSphere.radius = sqrt(50.0f*50.0f + 50.0f*50.0f);

	XMVECTOR lightDir = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
	XMVECTOR lightPos = 2.0*bSphere.radius*lightDir;
	XMVECTOR targetPos = XMLoadFloat3(&bSphere.center);

	XMMATRIX V = XMMatrixLookAtLH(lightPos, targetPos, XMLoadFloat3(&this->mUp));

	//Transform bounding sphere to light space
	XMFLOAT3 sphereCenterLS;
	XMStoreFloat3(&sphereCenterLS, XMVector3TransformCoord(targetPos, V));

	//Orthographic frustum in light space
	float l = sphereCenterLS.x - bSphere.radius;
	float r = sphereCenterLS.x + bSphere.radius;
	float b = sphereCenterLS.y - bSphere.radius;
	float t = sphereCenterLS.y + bSphere.radius;
	this->mNearZ = sphereCenterLS.z - bSphere.radius;
	this->mFarZ = sphereCenterLS.z + bSphere.radius;

	XMMATRIX P = XMMatrixOrthographicOffCenterLH(l, r, b, t, this->mNearZ, this->mFarZ);

	XMStoreFloat4x4(&this->mView, V);
	XMStoreFloat4x4(&this->mProj, P);
}

XMFLOAT4X4 Sun::getView() const
{
	return this->mView;
}

XMMATRIX Sun::getViewXM() const
{
	return XMLoadFloat4x4(&this->mView);
}

XMFLOAT4X4 Sun::getProj() const
{
	return this->mProj;
}

XMMATRIX Sun::getProjXM() const
{
	return XMLoadFloat4x4(&this->mProj);
}