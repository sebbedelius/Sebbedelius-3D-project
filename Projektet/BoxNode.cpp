#include "BoxNode.h"

BoxNode::BoxNode(int nrOfPolygonsInScene)
{
	this->children = new BoxNode*[4];

	for (int i = 0; i < 4; i++)
	{
		this->children[i] = nullptr;
	}
	
	polygonList.resize(nrOfPolygonsInScene);
	
	for (int i = 0; i < nrOfPolygonsInScene; i++)
	{
		polygonList[i] = -1;
	}
}

BoxNode::~BoxNode()
{
	for (int i = 0; i < 4; i++)
	{
		delete this->children[i];
	}

	delete[] this->children;	
}

void BoxNode::initializeChildren(int nrOfPolygonsInScene)
{
	for (int i = 0; i < 4; i++)
	{
		this->children[i] = new BoxNode(nrOfPolygonsInScene);
	}
}

void BoxNode::addPolygon(int id)
{
	this->polygonList[this->nrOfIds++] = id;
}

int BoxNode::getNrOfIds() const
{
	return this->nrOfIds;
}

void BoxNode::setNrOfIds(int nr)
{
	this->nrOfIds = nr;
}
