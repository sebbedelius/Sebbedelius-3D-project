#ifndef WAVES_H
#define WAVES_H
#include <vector>
#include <d3d11.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <chrono>
using namespace DirectX;

class Waves
{
private:
	std::chrono::high_resolution_clock::time_point start;

	XMMATRIX world;

	std::vector<XMFLOAT3> currentVertices;
	std::vector<XMFLOAT3> oldVertices;

	XMFLOAT3 mPosition;

	int nrOfVertices;
	int nrOfTriangles;
	int nrOfRows;
	int nrOfCols;
	
	int id;	
	bool draw;

	//Simulation constants
	float mk1, mk2, mk3;

	float mTimeStep;
	float mSpatialStep;

	BoundingBox mBB;

public:
	Waves();
	~Waves();

	void initialize(XMFLOAT3 position, int width, int depth, float spatialStep, float dt, float speed, float damping);

	void createBoundingBox();

	int getNrOfVertices() const;
	int getNrOfTriangles() const;
	int getNrOfRows() const;
	int getNrOfCols() const;
	int getId() const;
	bool getDraw() const;
	
	std::vector<XMFLOAT3> getCurrentVertices() const;
	BoundingBox getBoundingBox() const;
	XMMATRIX getWorld() const;	

	float width() const;
	float depth() const;

	void setId(int id);
	void setDraw(bool value);

	void update();
	void disturb(int i, int j, float magnitude);
};

#endif