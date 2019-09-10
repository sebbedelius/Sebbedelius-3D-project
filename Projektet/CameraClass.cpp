#include "CameraClass.h"

void CameraClass::checkMouseMovement(float dt)
{
	float x, y;
	float dx, dy;

	if (GetFocus() == this->wndHandle)
	{
		if (GetCursorPos(&this->mMouse))
		{
			x = this->mMouse.x;
			y = this->mMouse.y;

			ShowCursor(false);

			dx = XMConvertToRadians(this->sensitivity * static_cast<float>(x - this->lastPosX));
			dy = XMConvertToRadians(this->sensitivity * static_cast<float>(y - this->lastPosY));

			this->pitch(dy);
			this->yaw(dx);

			SetCursorPos(this->middleWidth, this->middleHeight);
			this->lastPosX = this->middleWidth;
			this->lastPosY = this->middleHeight;
		}		
	}
}

void CameraClass::calculateWindowMiddle()
{
	RECT wndRect;

	GetWindowRect(this->wndHandle, &wndRect);

	this->middleWidth = (wndRect.right - wndRect.left) / 2;
	this->middleHeight = (wndRect.bottom - wndRect.top) / 2;	
}

CameraClass::CameraClass(HWND hndWnd)
{
	this->wndHandle = hndWnd;
	
	this->noclip = Off;
	this->isStatic = false;

	this->mPosition = XMFLOAT3(15.0f, 15.0f, 0.0f); 
	this->mRight = XMFLOAT3(1.0f, 0.0f, 0.0f);
	this->mUp = XMFLOAT3(0.0f, 1.0f, 0.0f);
	this->mLook = XMFLOAT3(0.0f, 0.0f, 0.1f);
	this->mForward = XMFLOAT3(0.0f, 0.0f, 1.0f);	

	this->mNearWindowHeight = this->getNearWindowHeight();
	this->mFarWindowHeight = this->getFarWindowHeight();

	this->updateView();
	this->setLens(PI * 0.45f, WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 500.0f);

	this->calculateWindowMiddle();
	SetCursorPos(this->middleWidth, this->middleHeight);
	this->lastPosX = this->middleWidth;
	this->lastPosY = this->middleHeight;

	this->sensitivity = 0.08f;
	this->moveSpeed = 15.0f;
	this->flySpeed = 40.0f;

	this->start = std::chrono::steady_clock::now();	
}

CameraClass::~CameraClass()
{
}

void CameraClass::setLens(float fovY, float aspect, float zn, float zf)
{
	this->mFovY = fovY;
	this->mAspectRatio = aspect;

	this->mNearZ = zn;
	this->mFarZ = zf;

	this->mNearWindowHeight = 2.0f * this->mNearZ * tanf(0.5f * this->mFovY);
	this->mFarWindowHeight = 2.0f * this->mFarZ * tanf(0.5f * this->mFovY);

	XMMATRIX P = XMMatrixPerspectiveFovLH(this->mFovY, this->mAspectRatio, this->mNearZ, this->mFarZ);

	XMStoreFloat4x4(&this->mProj, P);
}

void CameraClass::lookAt(FXMVECTOR pos, FXMVECTOR target, FXMVECTOR worldUp)
{
	XMVECTOR L = XMVector3Normalize(XMVectorSubtract(target, pos));
	XMVECTOR R = XMVector3Normalize(XMVector3Cross(worldUp, L));
	XMVECTOR U = XMVector3Normalize(XMVector3Cross(L, R));

	XMStoreFloat3(&this->mLook, L);
	XMStoreFloat3(&this->mRight, R);
	XMStoreFloat3(&this->mUp, U);
}

void CameraClass::lookAt(const XMFLOAT3 & pos, const XMFLOAT3 & target, const XMFLOAT3 & up)
{
}

FlyCam CameraClass::updateAndGetNoclip() 
{
	auto end = std::chrono::steady_clock::now();
	float dt = std::chrono::duration<float>(end - this->start).count(); //Time from the last time we pressed a function key

	if (dt > 1.0f && GetAsyncKeyState(DirectX::Keyboard::LeftControl)
		|| GetAsyncKeyState(DirectX::Keyboard::RightControl))
	{
		if (this->noclip == On)
		{
			this->noclip = Off;
		}
		else
		{
			this->noclip = On;
		}
		this->start = end;
	}

	return this->noclip;
}

bool CameraClass::updateAndGetStatic()
{
	auto end = std::chrono::steady_clock::now();
	float dt = std::chrono::duration<float>(end - this->start).count(); //Time from the last time we pressed a function key

	if (dt > 0.5f && GetAsyncKeyState(Keyboard::P))
	{
		if (this->isStatic == true)
		{
			this->isStatic = false;
		}
		else
		{
			this->isStatic = true;
		}
		this->start = end;
	}

	return this->isStatic;
}

XMVECTOR CameraClass::getPositionXM() const
{
	return XMLoadFloat3(&this->mPosition);
}

XMFLOAT3 CameraClass::getPosition() const
{
	return this->mPosition;
}

XMVECTOR CameraClass::getRightXM() const
{
	return XMLoadFloat3(&this->mRight);
}

XMFLOAT3 CameraClass::getRight() const
{
	return this->mRight;
}

XMVECTOR CameraClass::getUpXM() const
{
	return XMLoadFloat3(&this->mUp);
}

XMFLOAT3 CameraClass::getUp() const
{
	return this->mUp;
}

XMVECTOR CameraClass::getForwardXM() const
{
	return XMLoadFloat3(&this->mForward);
}

XMFLOAT3 CameraClass::getForward() const
{
	return this->mForward;
}

XMFLOAT3 CameraClass::getLook() const
{
	return this->mLook;
}

float CameraClass::getNearZ() const
{
	return this->mNearZ;
}

float CameraClass::getFarZ() const
{
	return this->mFarZ;
}

float CameraClass::getAspectRatio() const
{
	return this->mAspectRatio;
}

float CameraClass::getFovY() const
{
	return this->mFovY;
}

float CameraClass::getFovX() const
{
	float halfWidth = 0.5f * this->getNearWindowWidth();

	return 2.0f * atan(halfWidth / this->mNearZ);
}

float CameraClass::getNearWindowWidth() const
{
	return this->mAspectRatio * this->mNearWindowHeight;
}

float CameraClass::getFarWindowWidth() const
{
	return this->mAspectRatio * this->mFarWindowHeight;
}

float CameraClass::getNearWindowHeight() const
{
	return this->mNearWindowHeight;
}

float CameraClass::getFarWindowHeight() const
{
	return this->mFarWindowHeight;
}

XMMATRIX CameraClass::getView() const
{
	return XMLoadFloat4x4(&this->mView);
}

XMMATRIX CameraClass::getProj() const
{
	return XMLoadFloat4x4(&this->mProj);
}

void CameraClass::setPositionXM(XMVECTOR pos)
{
	XMStoreFloat3(&this->mPosition, pos);
}

void CameraClass::setPosition(XMFLOAT3 pos)
{
	this->mPosition = pos;
}

void CameraClass::setPositionX(float x)
{
	this->mPosition.x = x;
}

void CameraClass::setPositionZ(float z)
{
	this->mPosition.z = z;
}

void CameraClass::setHeight(float h)
{
	this->mPosition.y = h;
}

void CameraClass::checkKeysFly(float dt)
{
	if (GetAsyncKeyState(DirectX::Keyboard::D))
	{
		this->strafe(this->flySpeed * dt);
	}
	if (GetAsyncKeyState(DirectX::Keyboard::A))
	{
		this->strafe(-this->flySpeed * dt);
	}
	if (GetAsyncKeyState(DirectX::Keyboard::W))
	{
		this->walk(this->flySpeed * dt);
	}
	if (GetAsyncKeyState(DirectX::Keyboard::S))
	{
		this->walk(-this->flySpeed * dt);
	}
}

void CameraClass::checkKeysTerrainWalk(float dt)
{
	if (GetAsyncKeyState(DirectX::Keyboard::D))
	{
		this->strafe(this->moveSpeed * dt);
	}
	if (GetAsyncKeyState(DirectX::Keyboard::A))
	{
		this->strafe(-this->moveSpeed * dt);
	}
	if (GetAsyncKeyState(DirectX::Keyboard::W))
	{
		this->terrainWalk(this->moveSpeed * dt);
	}
	if (GetAsyncKeyState(DirectX::Keyboard::S))
	{
		this->terrainWalk(-this->moveSpeed * dt);
	}
}

void CameraClass::strafe(float d)
{
	//mPosition += d * mRight
	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR r = XMLoadFloat3(&this->mRight);
	XMVECTOR p = XMLoadFloat3(&this->mPosition);
	XMStoreFloat3(&this->mPosition, XMVectorMultiplyAdd(s, r, p));
}

void CameraClass::walk(float d)
{
	//mPosition += d*mLook
	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR l = XMLoadFloat3(&this->mLook);
	XMVECTOR p = XMLoadFloat3(&this->mPosition);
	XMStoreFloat3(&this->mPosition, XMVectorMultiplyAdd(s, l, p));
}

void CameraClass::terrainWalk(float d)
{
	//mPosition += d*mForward
	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR l = XMLoadFloat3(&this->mForward);
	XMVECTOR p = XMLoadFloat3(&this->mPosition);
	XMStoreFloat3(&this->mPosition, XMVectorMultiplyAdd(s, l, p));
}

void CameraClass::pitch(float angle)
{
	//Rotate up and look vector about the right vector
	XMMATRIX r = XMMatrixRotationAxis(XMLoadFloat3(&this->mRight), angle);

	XMStoreFloat3(&this->mUp, XMVector3TransformNormal(XMLoadFloat3(&this->mUp), r));
	XMStoreFloat3(&this->mLook, XMVector3TransformNormal(XMLoadFloat3(&this->mLook), r));
}

void CameraClass::yaw(float angle)
{
	//Rotate the basis vectors about the world y-axis
	XMMATRIX r = XMMatrixRotationY(angle);

	XMStoreFloat3(&this->mRight, XMVector3TransformNormal(XMLoadFloat3(&this->mRight), r));
	XMStoreFloat3(&this->mUp, XMVector3TransformNormal(XMLoadFloat3(&this->mUp), r));
	XMStoreFloat3(&this->mLook, XMVector3TransformNormal(XMLoadFloat3(&this->mLook), r));
	XMStoreFloat3(&this->mForward, XMVector3TransformNormal(XMLoadFloat3(&this->mForward), r));
}

void CameraClass::updateView()
{
	XMVECTOR R = XMLoadFloat3(&this->mRight);
	XMVECTOR U = XMLoadFloat3(&this->mUp);
	XMVECTOR L = XMLoadFloat3(&this->mLook);
	XMVECTOR P = XMLoadFloat3(&this->mPosition);

	//Orthonormalize right, up, and look vectors

	//Make look vector unit length
	L = XMVector3Normalize(L);

	//New normalized up vector
	U = XMVector3Normalize(XMVector3Cross(L, R));

	//New normalized right vector
	R = XMVector3Cross(U, L);

	//Fill in the view matrix entries

	float x = -XMVectorGetX(XMVector3Dot(P, R));
	float y = -XMVectorGetX(XMVector3Dot(P, U));
	float z = -XMVectorGetX(XMVector3Dot(P, L));

	XMStoreFloat3(&this->mRight, R);
	XMStoreFloat3(&this->mUp, U);
	XMStoreFloat3(&this->mLook, L);

	this->mView(0, 0) = this->mRight.x;
	this->mView(1, 0) = this->mRight.y;
	this->mView(2, 0) = this->mRight.z;
	this->mView(3, 0) = x;

	this->mView(0, 1) = this->mUp.x;
	this->mView(1, 1) = this->mUp.y;
	this->mView(2, 1) = this->mUp.z;
	this->mView(3, 1) = y;

	this->mView(0, 2) = this->mLook.x;
	this->mView(1, 2) = this->mLook.y;
	this->mView(2, 2) = this->mLook.z;
	this->mView(3, 2) = z;

	this->mView(0, 3) = 0.0f;
	this->mView(1, 3) = 0.0f;
	this->mView(2, 3) = 0.0f;
	this->mView(3, 3) = 1.0f;

}

void CameraClass::update(float dt)
{	
	this->checkMouseMovement(dt);
	this->updateView();
}