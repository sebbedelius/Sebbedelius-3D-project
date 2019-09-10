#ifndef BOXNODE_H
#define BOXNODE_H
#include <vector>
#include <DirectXCollision.h>
using namespace DirectX;

class BoxNode
{

private:
	int nrOfIds = 0; 	
public:
	BoxNode(int nrOfPolygonsInScene);
	~BoxNode();

	void initializeChildren(int nrOfPolygonsInScene);

	BoxNode** children;		 
	BoundingBox mBB;	
	std::vector<int> polygonList; //List of polygon id:s in this node

	void addPolygon(int id);
	int getNrOfIds() const;

	void setNrOfIds(int nr);
};

#endif
