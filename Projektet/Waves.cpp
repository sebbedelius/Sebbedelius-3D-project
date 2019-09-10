#include "Waves.h"

Waves::Waves()
{
	this->world = XMMatrixRotationY(0);
}

Waves::~Waves()
{
}

void Waves::initialize(XMFLOAT3 position, int m, int n, float spatialStep, float dt, float speed, float damping)
{
	this->mPosition = position;
	this->nrOfRows = m;
	this->nrOfCols = n;
	this->nrOfVertices = this->nrOfRows * this->nrOfCols;
	this->nrOfTriangles = (this->nrOfRows - 1)*(this->nrOfCols - 1) * 2;
	this->mTimeStep = dt;
	this->mSpatialStep = spatialStep;

	float d = damping * dt + 2.0f;
	float e = (speed*speed)*(dt*dt) / (spatialStep*spatialStep);

	mk1 = (damping*dt - 2.0f) / d;
	mk2 = (4.0f - 8.0f*e) / d;
	mk3 = (2.0f*e) / d;

	this->oldVertices.resize(this->nrOfRows* this->nrOfCols);
	this->currentVertices.resize(this->nrOfRows* this->nrOfCols);

	//Generate grid vertices
	float halfWidth = (n - 1)*spatialStep*0.5f;
	float halfDepth = (m - 1)*spatialStep*0.5f;

	for (int i = 0; i < m; i++)
	{
		float z = halfDepth - i * spatialStep + this->mPosition.z;

		for (int j = 0; j < n; j++)
		{
			float x = -halfWidth + j * spatialStep + this->mPosition.x;

			this->oldVertices[i*n + j] = XMFLOAT3(x, position.y, z);
			this->currentVertices[i*n + j] = XMFLOAT3(x, position.y, z);
		}
	}

	this->start = std::chrono::steady_clock::now();
}

void Waves::createBoundingBox()
{
	this->mBB.CreateFromPoints(this->mBB, this->nrOfVertices, &this->currentVertices[0], sizeof(XMFLOAT3));
}

void Waves::setId(int id)
{
	this->id = id;
}

void Waves::setDraw(bool value)
{
	this->draw = value;
}

int Waves::getNrOfVertices() const
{
	return this->nrOfVertices;
}

int Waves::getNrOfTriangles() const
{
	return this->nrOfTriangles;
}

int Waves::getNrOfRows() const
{
	return this->nrOfRows;
}

int Waves::getNrOfCols() const
{
	return this->nrOfCols;
}

std::vector<XMFLOAT3> Waves::getCurrentVertices() const
{
	return this->currentVertices;
}

BoundingBox Waves::getBoundingBox() const
{
	return this->mBB;
}

XMMATRIX Waves::getWorld() const
{
	return this->world;
}

int Waves::getId() const
{
	return this->id;
}

bool Waves::getDraw() const
{
	return this->draw;
}

float Waves::width() const
{
	return this->nrOfCols * this->mSpatialStep;
}

float Waves::depth() const
{
	return this->nrOfRows * this->mSpatialStep;
}

void Waves::update()
{
	auto end = std::chrono::steady_clock::now();
	float dt = std::chrono::duration<float>(end - this->start).count();

	if (dt >= this->mTimeStep)
	{
		for (int i = 1; i < this->nrOfRows - 1; i++)
		{
			for (int j = 1; j < this->nrOfCols - 1; j++)
			{
				this->oldVertices[i*this->nrOfCols + j].y = 
					this->mk1*this->oldVertices[i*this->nrOfCols + j].y +
					this->mk2*this->currentVertices[i*this->nrOfCols + j].y +
					this->mk3*(this->currentVertices[(i + 1)*this->nrOfCols + j].y +
						this->currentVertices[(i - 1)*this->nrOfCols + j].y +
						this->currentVertices[i*this->nrOfCols + j + 1].y +
						this->currentVertices[i*this->nrOfCols + j - 1].y);
			}
		}

		std::swap(this->oldVertices, this->currentVertices);

		this->start = end;
	}
}

void Waves::disturb(int i, int j, float magnitude)
{
	if ((i > 1 && i < this->nrOfRows - 2) 
		&& (j > 1 && j < this->nrOfCols - 2))
	{
		float halfMag = 0.5f * magnitude;

		//Disturb the ij:th vertex height and its neighbors
		this->currentVertices[i*this->nrOfCols + j].y += halfMag / 2.5;
		this->currentVertices[i*this->nrOfCols + j + 1].y += halfMag / 3;
		this->currentVertices[i*this->nrOfCols + j - 1].y += halfMag / 3;
		this->currentVertices[(i + 1)*this->nrOfCols + j].y += halfMag / 3;
		this->currentVertices[(i - 1)*this->nrOfCols + j].y += halfMag / 3;
	}
}
