#include "StaticCamera.h"

void StaticCamera::setLens()
{
	XMMATRIX V = XMMatrixLookAtLH(XMLoadFloat3(&this->mPosition), XMLoadFloat3(&this->mLook), XMLoadFloat3(&this->mUp));

	XMStoreFloat4x4(&this->mView, V);

	XMMATRIX P = XMMatrixPerspectiveFovLH(this->mFovY, this->mAspectRatio, this->mNearZ, this->mFarZ);

	XMStoreFloat4x4(&this->mProj, P);
}

StaticCamera::StaticCamera()
{
	this->mPosition = XMFLOAT3(10.0f, 20.0f, -50.0f);
	this->mUp = XMFLOAT3(0.0f, 1.0f, 0.0f);
	this->mLook = XMFLOAT3(0.0f, 0.0f, 0.1f);

	this->mNearZ = 0.1f;
	this->mFarZ = 500.0f;
	this->mAspectRatio = WINDOW_WIDTH / WINDOW_HEIGHT;
	this->mFovY = PI * 0.45f;

	this->setLens();
}

StaticCamera::~StaticCamera()
{
}

XMMATRIX StaticCamera::getView() const
{
	return XMLoadFloat4x4(&this->mView);
}

XMMATRIX StaticCamera::getProj() const
{
	return XMLoadFloat4x4(&this->mProj);
}
