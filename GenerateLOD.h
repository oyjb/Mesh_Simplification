#pragma once
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <fbxsdk.h>
#include <queue>
#include <set>

//Definition for Point
class Point {
public:
	// adjacent vertices
	// must be set, other than unordered_set can not use set_union() and set_difference()
	std::set<int> Vi;
	// adjacent triangles
	// must be set, other than unordered_set can not use set_union() and set_difference()
	std::set<int> Ti;
	// adjacent edges
	std::unordered_set<int> Ei;
	std::unordered_set<int> uvSet;
	// mark the vertex if hash only one uv coordinate
	bool isOneUV = true;
};
//Definition for Polygon
class Face {
public:
	std::vector<FbxDouble> normal;
	std::vector<int> points; // three index point to the array of controlpoints
	std::vector<int> uvs;    // three index point to the array of UVs
};
//Ddefinition for Edge
class Edge {
public:
	bool ShouldCollapse = true;
	bool isUV = true;
	std::vector<int> twoPoints; // two index point to the array of controlpoints
	FbxDouble Cost = 0.0;
};


class GenerateLOD
{
public:
	GenerateLOD();
	~GenerateLOD();

public:
	void LoadFbx();

	std::vector<FbxDouble> GetNormal(FbxVector4 *pControlPoints, int idx0, int idx1, int idx2);
	FbxDouble ComputeLength(FbxVector4 *pControlPoints, int u, int v);

	void Init_Triangles_And_ControlP_EdgeCollapse(FbxMesh *pMesh, FbxVector4 *pControlPoints);
	void InitEdges(FbxMesh *pMesh);

	void GetEdgeCost(bool &isUtoV, FbxDouble &cost, int U, int V, FbxVector4 *pControlPoints);
	void CollapseEdge_And_UpdateCost(int EdgeIdx, FbxVector4 *pControlPoints, int &addTriangleIndex, int &addEdgeIndex);

	FbxMesh *CreateNewMesh(FbxVector4 *pControlPoints, FbxMesh *pMesh, FbxScene *pScene);
	void Reduction_EdgesCollapse_UV(FbxNode *node, FbxMesh *pMesh, FbxManager *pManager, FbxScene *pScene);

	void Init_Buffer(fbxsdk::FbxMesh *pMesh, Model &model);
	void saveScene(const char *pFilename, FbxManager *pFbxManager, FbxScene *pFbxScene, bool pAsASCII);
	int getASCIIFormatIndex(FbxManager *pManager);

private:
	// Definition for Points Set
	std::unordered_map<int, Point> *ControlP;

	// Definition for Tiangles Set
	std::unordered_map<int, Face> *Triangles;

	// Definition for Edges Set
	std::unordered_map<int, Edge> *Edges;

	// Definition for the cost for each Edge
	std::unordered_map< double, std::vector<int>> *CostToEdge;

	// Definition for the cost list
	std::priority_queue<double, std::vector<double>, std::greater<double>> *HeapCost;
};