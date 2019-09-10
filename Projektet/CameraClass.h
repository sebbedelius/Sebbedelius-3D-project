#ifndef CAMERACLASS_H
#define CAMERACLASS_H
#include <d3d11.h>
#include <DirectXMath.h>
#include <memory>
#include "Keyboard.h"
#include "Mouse.h"
#include "GlobalVaribles.h"
#include <chrono>
using namespace DirectX;

//With help from the book "Introduction to 3D programming with DirectX 11" by Franc Luna
static const float MY_PI = 3.1415;
enum FlyCam {On, Off};

class CameraClass
{
private:
	HWND wndHandle;	
	
	std::chrono::high_resolution_clock::time_point start;
	FlyCam noclip;
	bool isStatic;

	XMFLOAT3 mPosition;
	XMFLOAT3 mRight;
	XMFLOAT3 mUp;
	XMFLOAT3 mLook;
	XMFLOAT3 mForward;

	float mNearZ;
	float mFarZ;
	float mAspectRatio;
	float mFovY;
	float mNearWindowHeight;
	float mFarWindowHeight;

	XMFLOAT4X4 mView;
	XMFLOAT4X4 mProj;

	//Mouse Pos
	int lastPosX, lastPosY;	
	std::unique_ptr<DirectX::Mouse> mMouseInfo;
	POINT mMouse;
	float sensitivity;
	float moveSpeed;
	float flySpeed;

	float middleWidth, middleHeight;

	//bool checkKeys(float dt);	
	void checkMouseMovement(float dt);
	void calculateWindowMiddle();

public:
	CameraClass(HWND hndWnd);
	~CameraClass();

	void setLens(float fovY, float aspect, float zn, float zf);

	void lookAt(FXMVECTOR pos, FXMVECTOR target, FXMVECTOR worldUp);
	void lookAt(const XMFLOAT3& pos, const XMFLOAT3& target, const XMFLOAT3& up);

	FlyCam updateAndGetNoclip();
	bool updateAndGetStatic();

	XMVECTOR getPositionXM() const;
	XMFLOAT3 getPosition() const;
	XMVECTOR getRightXM() const;
	XMFLOAT3 getRight() const;
	XMVECTOR getUpXM() const;
	XMFLOAT3 getUp() const;
	XMVECTOR getForwardXM() const;
	XMFLOAT3 getForward() const;
	
	XMFLOAT3 getLook() const;
	float getNearZ() const;
	float getFarZ() const;
	float getAspectRatio() const;
	float getFovY() const;
	float getFovX() const;

	float getNearWindowWidth() const;
	float getFarWindowWidth() const;
	float getNearWindowHeight() const;
	float getFarWindowHeight() const;

	XMMATRIX getView() const;
	XMMATRIX getProj() const;

	void setPositionXM(XMVECTOR pos);
	void setPosition(XMFLOAT3 pos);
	void setPositionX(float x);
	void setPositionZ(float z);
	void setHeight(float h);

	void checkKeysFly(float dt);
	void checkKeysTerrainWalk(float dt);

	void strafe(float d);
	void walk(float d);
	void terrainWalk(float d);

	void pitch(float angle);
	void yaw(float angle);

	void updateView();
	void update(float dt);
};

#endif
