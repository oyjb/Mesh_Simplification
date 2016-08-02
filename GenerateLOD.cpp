#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <algorithm>
#include <iterator>
#include <fbxsdk.h>
#include <d3dx9.h>
#include <string>
#include <vector>
#include <limits>
#include <queue>
#include <set>
#pragma warning( disable : 4996 ) // disable deprecated warning 
#include <strsafe.h>
#pragma warning( default : 4996 )
#include "Common.h"

GenerateLOD::GenerateLOD()
{
	ControlP = new std::unordered_map<int, Point>();
	Triangles = new std::unordered_map<int, Face>();
	Edges = new std::unordered_map<int, Edge>();
	CostToEdge = new std::unordered_map< double, std::vector<int>>();
	HeapCost = new std::priority_queue<double, std::vector<double>, std::greater<double>>();
}

GenerateLOD::~GenerateLOD()
{
	delete ControlP;
	delete Triangles;
	delete Edges;
	delete CostToEdge;
	delete HeapCost;
}

// Load Fbx File
void GenerateLOD::LoadFbx()
{
	FbxManager *fbxManager = FbxManager::Create();

	//Create an IOSetting
	FbxIOSettings *ios = FbxIOSettings::Create(fbxManager, IOSROOT);
	fbxManager->SetIOSettings(ios);

	//Create an impoter
	FbxImporter *lImporter = FbxImporter::Create(fbxManager, "myImporter");
	std::string tmp = std::string(".\\LODs\\") + srcFbxName;
	bool lImporterStatus = lImporter->Initialize(tmp.c_str(), -1, fbxManager->GetIOSettings());
	if (!lImporterStatus) {
		MessageBox(NULL, "No Scuh File in .\\LODs\\ directory !", "Warning", 0);
		return;
	}

	FbxScene *fbxScene = FbxScene::Create(fbxManager, "myScene");
	lImporter->Import(fbxScene);

	FbxNode *rootNode = fbxScene->GetRootNode();
	if (rootNode != NULL) {
		for (int i = 0; i < rootNode->GetChildCount(); ++i) {
			FbxNode *node = rootNode->GetChild(i);
			FbxNodeAttribute *Att = node->GetNodeAttribute();
			if (Att != NULL && Att->GetAttributeType() == FbxNodeAttribute::eMesh) {
				FbxMesh *lMesh = (FbxMesh *)(Att);
				//FbxMesh *lMesh = dynamic_cast<FbxMesh *>(Att);
				if (!lMesh->IsTriangleMesh()) {
					FbxGeometryConverter converter = FbxGeometryConverter(fbxManager);
					FbxNodeAttribute *Attr = converter.Triangulate(lMesh, true);
					lMesh = (FbxMesh *)(Attr);
				}

				//Following is the SImplification
				Reduction_EdgesCollapse_UV(node, lMesh, fbxManager, fbxScene);

			}
		}
	}

	//MessageBox(NULL, "Export Succeed!", "Export", 0);
	fbxManager->Destroy();
}

// Compute normal for a triangle
std::vector<FbxDouble> GenerateLOD::GetNormal(FbxVector4 *pControlPoints, int idx0, int idx1, int idx2)
{
	FbxVector4 p1 = pControlPoints[idx0];
	FbxVector4 p2 = pControlPoints[idx1];
	FbxVector4 p3 = pControlPoints[idx2];
	std::vector<FbxDouble> vec1 = { p1[0] - p2[0],
		p1[1] - p2[1],
		p1[2] - p2[2] };
	std::vector<FbxDouble> vec2 = { p2[0] - p3[0],
		p2[1] - p3[1],
		p2[2] - p3[2] };
	std::vector<FbxDouble> vec1_X_vec2 = { vec1[1] * vec2[2] - vec1[2] * vec2[1],
		vec1[2] * vec2[0] - vec1[0] * vec2[2],
		vec1[0] * vec2[1] - vec1[1] * vec2[0] };
	FbxDouble tmp = sqrt(vec1_X_vec2[0] * vec1_X_vec2[0] +
		vec1_X_vec2[1] * vec1_X_vec2[1] +
		vec1_X_vec2[2] * vec1_X_vec2[2]);

	std::vector<FbxDouble> Normal = { vec1_X_vec2[0] / tmp, vec1_X_vec2[1] / tmp, vec1_X_vec2[2] / tmp };
	return Normal;
}

// Init the ditc of (*Triangles) and the dict of (*ControlP)
void GenerateLOD::Init_Triangles_And_ControlP_EdgeCollapse(FbxMesh *pMesh, FbxVector4 *pControlPoints)
{
	int triangleCounts = pMesh->GetPolygonCount();
	int ElementUVCount = pMesh->GetElementUVCount();
	if (ElementUVCount < 1) {
		MessageBox(NULL, "Do not read the related uv information", "message", 0);
	}
	for (int i = 0; i < triangleCounts; ++i) {
		int cp0 = pMesh->GetPolygonVertex(i, 0);
		int cp1 = pMesh->GetPolygonVertex(i, 1);
		int cp2 = pMesh->GetPolygonVertex(i, 2);

		//(*Triangles)[i] = Face(); //Init a Polygon for i'th triangle
		(*Triangles)[i].points = { cp0, cp1, cp2 };
		//std::vector<FbxDouble> n = GetNormal(pControlPoints, cp0, cp1, cp2);
		(*Triangles)[i].normal = GetNormal(pControlPoints, cp0, cp1, cp2);

		// Set the three vertex's uv coordinate
		int uvindex0 = pMesh->GetTextureUVIndex(i, 0);
		int uvindex1 = pMesh->GetTextureUVIndex(i, 1);
		int uvindex2 = pMesh->GetTextureUVIndex(i, 2);

		(*Triangles)[i].uvs = { uvindex0, uvindex1, uvindex2 };

		// for Controlp[cp0]
		//if ((*ControlP).find(cp0) == (*ControlP).end())
		//(*ControlP)[cp0] = Point();
		Point &P0 = (*ControlP)[cp0];
		// uv, Ti, Vi for (*ControlP)[cp0]
		P0.uvSet.insert(uvindex0);
		P0.Ti.insert(i);
		P0.Vi.insert(cp1);
		P0.Vi.insert(cp2);

		// for Controlp[cp1]
		//if ((*ControlP).find(cp1) == (*ControlP).end())
		//(*ControlP)[cp1] = Point();
		Point &P1 = (*ControlP)[cp1];
		// uv, Ti, Vi for (*ControlP)[cp0]
		P1.uvSet.insert(uvindex1);
		P1.Ti.insert(i);
		P1.Vi.insert(cp0);
		P1.Vi.insert(cp2);

		// for Controlp[cp2]
		//if ((*ControlP).find(cp2) == (*ControlP).end())
		//(*ControlP)[cp2] = Point();
		Point &P2 = (*ControlP)[cp2];
		// uv, Ti, Vi for (*ControlP)[cp0]
		P2.uvSet.insert(uvindex2);
		P2.Ti.insert(i);
		P2.Vi.insert(cp0);
		P2.Vi.insert(cp1);
	}
}

FbxDouble GenerateLOD::ComputeLength(FbxVector4 *pControlPoints, int u, int v)
{
	return sqrt(pow(pControlPoints[v][0] - pControlPoints[u][0], 2) +
		pow(pControlPoints[v][1] - pControlPoints[u][1], 2) +
		pow(pControlPoints[v][2] - pControlPoints[u][2], 2));
}

void GenerateLOD::InitEdges(FbxMesh *pMesh)
{
	FbxVector4 *pControlPoints = pMesh->GetControlPoints();
	int EdgeCount = pMesh->GetMeshEdgeCount();

	pMesh->BeginGetMeshEdgeVertices();
	for (int i = 0; i < EdgeCount; ++i) {
		int U;
		int V;
		pMesh->GetMeshEdgeVertices(i, U, V);
		(*ControlP)[U].Ei.insert(i);
		(*ControlP)[V].Ei.insert(i);

		//(*Edges)[i] = Edge();
		(*Edges)[i].twoPoints = { U, V };

		if ((*ControlP)[U].uvSet.size() > 1 || (*ControlP)[V].uvSet.size() > 1) {
			(*Edges)[i].ShouldCollapse = false;
			continue;
		}

		//MessageBox(NULL, "InitEdge1", "This", 0);

		FbxDouble LUV = ComputeLength(pControlPoints, U, V);

		// U ---> V
		FbxDouble fu = -1;
		const std::set<int> &UTi = (*ControlP)[U].Ti;
		const std::set<int> &VTi = (*ControlP)[V].Ti;
		std::unordered_set<int> Tuv;
		Tuv.clear();
		// Set Tuv as the union for (*ControlP)[U].Ti and (*ControlP)[V].Ti
		std::set_union(UTi.begin(), UTi.end(),
			VTi.begin(), VTi.end(),
			std::inserter(Tuv, Tuv.begin()));

		//MessageBox(NULL, "InitEdge2", "This", 0);

		for (const auto &i1 : (*ControlP)[U].Ti) {
			Tuv.erase(i1); // remove i1 from Tuv
			FbxDouble minfuv = (std::numeric_limits<double>::max)();
			const std::vector<FbxDouble> &normal_i = (*Triangles)[i1].normal;
			for (const auto &j : Tuv) {
				const std::vector<FbxDouble> &normal_j = (*Triangles)[j].normal;
				// (1 - f.normal * n.normal) / 2
				FbxDouble ijnorlmal = (1 - (
					normal_i[0] * normal_j[0] +
					normal_i[1] * normal_j[1] +
					normal_i[2] * normal_j[2])) / 2.0;
				if (ijnorlmal < minfuv)
					minfuv = ijnorlmal;
			}
			if (minfuv > fu)
				fu = minfuv;
			Tuv.insert(i1); // add back i1 to Tuv
		}
		FbxDouble costUV = fu;

		//MessageBox(NULL, "InitEdge3", "This", 0);

		// V ---> U
		FbxDouble fv = -1;
		for (const auto &i1 : (*ControlP)[V].Ti) {
			Tuv.erase(i1); // remove i1 from Tuv
			FbxDouble minfvu = (std::numeric_limits<double>::max)();
			const std::vector<FbxDouble> &normal_i = (*Triangles)[i1].normal;
			for (const auto &j : Tuv) {
				const std::vector<FbxDouble> &normal_j = (*Triangles)[j].normal;
				// (1 - f.normal * n.normal) / 2
				FbxDouble ijnorlmal = (1 - (
					normal_i[0] * normal_j[0] +
					normal_i[1] * normal_j[1] +
					normal_i[2] * normal_j[2])) / 2.0;
				if (ijnorlmal < minfvu)
					minfvu = ijnorlmal;
			}
			if (minfvu > fv)
				fv = minfvu;
			Tuv.insert(i1); // add back i1 to Tuv
		}
		FbxDouble costVU = fv;

		//MessageBox(NULL, "InitEdge4", "This", 0);

		if (costVU < costUV) {
			FbxDouble cost = LUV * costVU;
			(*Edges)[i].isUV = false;
			(*Edges)[i].Cost = cost;
			HeapCost->push(cost);
			//if ((*CostToEdge).find(cost) != (*CostToEdge).end())
			(*CostToEdge)[cost].push_back(i);
			//else
			//(*CostToEdge)[cost] = std::vector<int>({ i });
		}
		else {
			FbxDouble cost = LUV * costUV;
			(*Edges)[i].isUV = true;
			(*Edges)[i].Cost = cost;
			HeapCost->push(cost);
			//if ((*CostToEdge).find(cost) != (*CostToEdge).end())
			(*CostToEdge)[cost].push_back(i);
			//else
			//(*CostToEdge)[cost] = std::vector<int>({ i });
		}
	}
	pMesh->EndGetMeshEdgeVertices();
}

void GenerateLOD::GetEdgeCost(bool &isUtoV, FbxDouble &cost, int U, int V, FbxVector4 *pControlPoints)
{
	FbxDouble LUV = ComputeLength(pControlPoints, U, V);
	const std::set<int> &UTi = (*ControlP)[U].Ti;
	const std::set<int> &VTi = (*ControlP)[V].Ti;
	std::unordered_set<int> Tuv;
	Tuv.clear();
	// Set Tuv as the union for (*ControlP)[U].Ti and (*ControlP)[V].Ti
	std::set_union(UTi.begin(), UTi.end(),
		VTi.begin(), VTi.end(),
		std::inserter(Tuv, Tuv.begin()));

	// U ---> V
	FbxDouble fu = -1;
	for (const auto &i1 : (*ControlP)[U].Ti) {
		Tuv.erase(i1); // remove i1 from Tuv
		FbxDouble minfuv = (std::numeric_limits<double>::max)();
		const std::vector<FbxDouble> &normal_i = (*Triangles)[i1].normal;
		for (const auto &j : Tuv) {
			const std::vector<FbxDouble> &normal_j = (*Triangles)[j].normal;
			// (1 - f.normal * n.normal) / 2
			FbxDouble ijnorlmal = (1 - (
				normal_i[0] * normal_j[0] +
				normal_i[1] * normal_j[1] +
				normal_i[2] * normal_j[2])) / 2.0;
			if (ijnorlmal < minfuv)
				minfuv = ijnorlmal;
		}
		if (minfuv > fu)
			fu = minfuv;
		Tuv.insert(i1); // add back i1 to Tuv
	}
	//FbxDouble costUV = LUV * fu;

	// V ---> U
	FbxDouble fv = -1;
	for (const auto &i1 : (*ControlP)[V].Ti) {
		Tuv.erase(i1); // remove i1 from Tuv
		FbxDouble minfvu = (std::numeric_limits<double>::max)();
		const std::vector<FbxDouble> &normal_i = (*Triangles)[i1].normal;
		for (const auto &j : Tuv) {
			if (j == i1)
				continue;
			const std::vector<FbxDouble> &normal_j = (*Triangles)[j].normal;
			// (1 - f.normal * n.normal) / 2
			FbxDouble ijnorlmal = (1 - (
				normal_i[0] * normal_j[0] +
				normal_i[1] * normal_j[1] +
				normal_i[2] * normal_j[2])) / 2.0;
			if (ijnorlmal < minfvu)
				minfvu = ijnorlmal;
		}
		if (minfvu > fv)
			fv = minfvu;
		Tuv.insert(i1); // add back i1 to Tuv
	}
	//FbxDouble costVU = LUV * fv;

	if (fv < fu) {
		isUtoV = false;
		cost = LUV * fv;
	}
	else {
		isUtoV = true;
		cost = LUV * fu;
	}
}

void GenerateLOD::CollapseEdge_And_UpdateCost(int EdgeIdx, FbxVector4 *pControlPoints, int &addTriangleIndex, int &addEdgeIndex)
{
	int U, V;
	const Edge &edge = (*Edges)[EdgeIdx];
	if (edge.isUV) {
		U = edge.twoPoints[0];
		V = edge.twoPoints[1];
	}
	else {
		U = edge.twoPoints[1];
		V = edge.twoPoints[0];
	}
	// Construct triangles in U's adjacent vertices
	const std::set<int> &UTi = (*ControlP)[U].Ti;
	const std::set<int> &VTi = (*ControlP)[V].Ti;
	std::unordered_set<int> UdiffV;
	UdiffV.clear();
	std::set_difference(UTi.begin(), UTi.end(),
		VTi.begin(), VTi.end(),
		std::inserter(UdiffV, UdiffV.begin()));

	for (const auto &i : UdiffV) {
		std::vector<int> TrianglethreePoints;
		std::vector<int> TrianglethreeUVs;

		const Face &trian_i = (*Triangles)[i];

		int flag;
		for (int j = 0; j < 3; ++j) {
			if (U == trian_i.points[j]) {
				flag = j;
				break;
			}
		}

		switch (flag)
		{
		case 2:
		{
			TrianglethreePoints.push_back(trian_i.points[0]);
			TrianglethreeUVs.push_back(trian_i.uvs[0]);

			TrianglethreePoints.push_back(trian_i.points[1]);
			TrianglethreeUVs.push_back(trian_i.uvs[1]);

			TrianglethreePoints.push_back(V);
			TrianglethreeUVs.push_back(*((*ControlP)[V].uvSet.begin()));

			break;
		}
		case 1:
		{
			TrianglethreePoints.push_back(trian_i.points[0]);
			TrianglethreeUVs.push_back(trian_i.uvs[0]);

			TrianglethreePoints.push_back(V);
			TrianglethreeUVs.push_back(*((*ControlP)[V].uvSet.begin()));

			TrianglethreePoints.push_back(trian_i.points[2]);
			TrianglethreeUVs.push_back(trian_i.uvs[2]);

			break;
		}
		case 0:
		{
			TrianglethreePoints.push_back(trian_i.points[1]);
			TrianglethreeUVs.push_back(trian_i.uvs[1]);

			TrianglethreePoints.push_back(trian_i.points[2]);
			TrianglethreeUVs.push_back(trian_i.uvs[2]);

			TrianglethreePoints.push_back(V);
			TrianglethreeUVs.push_back(*((*ControlP)[V].uvSet.begin()));

			break;
		}
		default:
			break;
		}

		//(*Triangles)[addTriangleIndex] = Face();
		Face &face = (*Triangles)[addTriangleIndex];
		face.points = TrianglethreePoints;
		face.uvs = TrianglethreeUVs;
		face.normal = GetNormal(pControlPoints,
			TrianglethreePoints[0],
			TrianglethreePoints[1],
			TrianglethreePoints[2]);
		(*ControlP)[TrianglethreePoints[0]].Ti.insert(addTriangleIndex);
		(*ControlP)[TrianglethreePoints[1]].Ti.insert(addTriangleIndex);
		(*ControlP)[TrianglethreePoints[2]].Ti.insert(addTriangleIndex);

		addTriangleIndex += 1;
	}

	//MessageBox(NULL, "OK5", "This", 0);

	// Delete all U's adjacent triangles from Dict (*Triangles)
	// And delete it in Is's adjacent vertex's Ti
	for (const auto &triangleIndex : (*ControlP)[U].Ti) {
		for (const auto &idx : (*Triangles)[triangleIndex].points) {
			if (idx != U)
				(*ControlP)[idx].Ti.erase(triangleIndex);
		}
		(*Triangles).erase(triangleIndex);
	}

	for (const auto &i : (*ControlP)[U].Vi) {
		//MessageBox(NULL, "OK a1", "This", 0);
		(*ControlP)[i].Vi.erase(U);
		//MessageBox(NULL, "OK a2", "This", 0);
	}

	//# # Delete (*Edges) from U's adjacent vertex's Ei
	for (const auto &i : (*ControlP)[U].Ei) {
		const Edge &edge_i = (*Edges)[i];
		if (U == edge_i.twoPoints[0])
			(*ControlP)[edge_i.twoPoints[1]].Ei.erase(i);
		else
			(*ControlP)[edge_i.twoPoints[0]].Ei.erase(i);

		if (edge_i.ShouldCollapse) {
			const FbxDouble &cost = edge_i.Cost;
			std::vector<int>::iterator it = find((*CostToEdge)[cost].begin(), (*CostToEdge)[cost].end(), i);
			(*CostToEdge)[cost].erase(it);
		}

		(*Edges).erase(i);  //# include edge U <----> V
	}

	//MessageBox(NULL, "OK8", "This", 0);

	// Construct new edges
	const std::set<int> &UVi = (*ControlP)[U].Vi;
	const std::set<int> &VVi = (*ControlP)[V].Vi;
	UdiffV.clear();
	std::set_difference(UVi.begin(), UVi.end(),
		VVi.begin(), VVi.end(),
		std::inserter(UdiffV, UdiffV.begin()));
	UdiffV.erase(V);
	for (const auto &i : UdiffV) {
		Edge &edge_i = (*Edges)[addEdgeIndex];
		//(*Edges)[addEdgeIndex] = Edge();
		edge_i.twoPoints = { i, V };

		(*ControlP)[i].Ei.insert(addEdgeIndex);
		(*ControlP)[V].Ei.insert(addEdgeIndex);

		(*ControlP)[i].Vi.insert(V);
		(*ControlP)[V].Vi.insert(i);

		if ((*ControlP)[i].uvSet.size() > 1) {
			edge_i.ShouldCollapse = false;
			addEdgeIndex += 1;
			continue;
		}
		bool isUtoV;
		FbxDouble cost;
		GetEdgeCost(isUtoV, cost, i, V, pControlPoints);
		edge_i.Cost = cost;
		edge_i.isUV = isUtoV;

		HeapCost->push(cost);
		//if ((*CostToEdge).find(cost) != (*CostToEdge).end())
		(*CostToEdge)[cost].push_back(addEdgeIndex);
		//else
		//(*CostToEdge)[cost] = std::vector<int>({addEdgeIndex});
		addEdgeIndex += 1;
	}
	(*ControlP).erase(U);
}

FbxMesh* GenerateLOD::CreateNewMesh(FbxVector4 *pControlPoints, FbxMesh *pMesh, FbxScene *pScene)
{
	// ********************************************************************************* //
	// Following is Create new Mesh
	FbxMesh *newMesh = FbxMesh::Create(pScene, "newMesh");
	newMesh->InitControlPoints((*ControlP).size());

	int count = 0;
	std::unordered_map<int, int> RemainPoints;
	for (std::unordered_map<int, Point>::iterator it = (*ControlP).begin(); it != (*ControlP).end(); ++it) {
		newMesh->SetControlPointAt(pControlPoints[it->first], count);
		RemainPoints[it->first] = count;
		++count;
	}

	FbxGeometryElementUV *newElementUV = newMesh->CreateElementUV("DiffuseUV");
	if (!newElementUV) {
		MessageBox(NULL, "CreateElementUV Error!", "ÌבÊ¾", 0);
		//exit(0);
	}

	newElementUV->SetMappingMode(FbxLayerElement::eByPolygonVertex);
	newElementUV->SetReferenceMode(FbxLayerElement::eIndexToDirect);

	//# copy oldElementUV.DirectArray() into newElementUV.DirectArray()
	FbxGeometryElementUV *oldElementUV = pMesh->GetElementUV(0);
	int length = oldElementUV->GetDirectArray().GetCount();
	for (int i = 0; i < length; ++i)
		newElementUV->GetDirectArray().Add(oldElementUV->GetDirectArray()[i]);

	//# Now we have set the UVs as eIndexToDirect reference and
	//# in eByPolygonVertex mapping mode, we must Set the size of the index array.
	int PolygonCount = (*Triangles).size();
	newElementUV->GetIndexArray().SetCount(3 * PolygonCount);

	//# Add new triangles to pMesh
	int newPolygonIndex = 0;
	for (std::unordered_map<int, Face>::iterator it = (*Triangles).begin(); it != (*Triangles).end(); ++it) {
		newMesh->BeginPolygon(-1, -1, -1, false);

		newMesh->AddPolygon(RemainPoints[it->second.points[0]], it->second.uvs[0]);
		newElementUV->GetIndexArray().SetAt(newPolygonIndex * 3 + 0, it->second.uvs[0]);

		newMesh->AddPolygon(RemainPoints[it->second.points[1]], it->second.uvs[1]);
		newElementUV->GetIndexArray().SetAt(newPolygonIndex * 3 + 1, it->second.uvs[1]);

		newMesh->AddPolygon(RemainPoints[it->second.points[2]], it->second.uvs[2]);
		newElementUV->GetIndexArray().SetAt(newPolygonIndex * 3 + 2, it->second.uvs[2]);

		newMesh->EndPolygon();
		newPolygonIndex += 1;
	}

	//# Automatically generate edge data for the mesh.
	newMesh->BuildMeshEdgeArray();

	return newMesh;
}

/*
FbxMesh pMesh : the original mesh load from fbx file
FbxScene pScene : the FbxScene load from fbx file
float threshold :
*/
void GenerateLOD::Reduction_EdgesCollapse_UV(FbxNode *node, FbxMesh *pMesh, FbxManager *pManager, FbxScene *pScene)
{
	// Clear the ralated data structure
	(*ControlP).clear();
	(*Triangles).clear();
	(*Edges).clear();
	(*CostToEdge).clear();
	// Clear the HeapCost
	while (!HeapCost->empty())
		HeapCost->pop();

	FbxVector4 *pControlPoints = pMesh->GetControlPoints();
	int AddTriangleIndex = pMesh->GetPolygonCount();
	int AddEdgeIndex = pMesh->GetMeshEdgeCount();

	int threshold_1 = static_cast<int>(AddTriangleIndex * model_1.GetReductionRate());
	int threshold_2 = static_cast<int>(AddTriangleIndex * model_2.GetReductionRate());
	int threshold_3 = static_cast<int>(AddTriangleIndex * model_3.GetReductionRate());

	double start = GetTickCount();
	//# Init the ditc of (*Triangles) and the dict of (*ControlP)
	Init_Triangles_And_ControlP_EdgeCollapse(pMesh, pControlPoints);/////////////////////////////

	//Before Reduction, Now Init the Original VertexBuffer
	Init_Buffer(pMesh, model_0);

	//# Init the ditc of (*Edges)
	InitEdges(pMesh); //////////////////////////////////

					  // Now, Simplify Iterator
	int Ring = 0;
	bool flag1 = true;
	bool flag2 = true;
	while (true) {
		int idx;
		if (0 == HeapCost->size())
			idx = -1;
		else {
			//First Get the top element, namely the minmum cost
			//Second pop the minmum cost from HeapCost
			FbxDouble PopCost = HeapCost->top();
			HeapCost->pop();
			if (0 == (*CostToEdge)[PopCost].size())
				continue;
			idx = (*CostToEdge)[PopCost][0];
		}

		//int idx = GetMinEdgeCost();
		if (-1 == idx) {
			MessageBox(NULL,
				"When execute Simplification, there has No any edges should be collapse.",
				"The title", 0);
			break;
		}

		// Execute the current Edge Collapse and Update the (*ControlP), (*Triangles), (*Edges).
		CollapseEdge_And_UpdateCost(idx, pControlPoints, AddTriangleIndex, AddEdgeIndex);

		Ring += 1;

		if (flag1 && (int)(*Triangles).size() < threshold_1) {
			// After Reduction, Now Init Reduction VertexBuffer
			Init_Buffer(pMesh, model_1);
			model = model_1;

			FbxMesh *newMesh = CreateNewMesh(pControlPoints, pMesh, pScene);
			// Update the Node Atrribute
			node->RemoveNodeAttribute(pMesh);
			node->SetNodeAttribute(newMesh);
			// Export new Mesh
			std::string tmp = std::string(".\\LODs\\") + std::string("Save") + srcFbxName.substr(0, srcFbxName.size() - 4) + "_(" + std::to_string(int(model_1.GetReductionRate() * 100)) + "%)" + ".FBX";
			saveScene(tmp.c_str(), pManager, pScene, true);

			node->RemoveNodeAttribute(newMesh);
			node->SetNodeAttribute(pMesh);
			flag1 = false;
		}

		if (flag2 && (int)(*Triangles).size() < threshold_2) {
			// After Reduction, Now Init Reduction VertexBuffer
			Init_Buffer(pMesh, model_2);

			FbxMesh *newMesh = CreateNewMesh(pControlPoints, pMesh, pScene);
			// Update the Node Atrribute
			node->RemoveNodeAttribute(pMesh);
			node->SetNodeAttribute(newMesh);
			// Export new Mesh
			std::string tmp = std::string(".\\LODs\\") + std::string("Save") + srcFbxName.substr(0, srcFbxName.size() - 4) + "_(" + std::to_string(int(model_2.GetReductionRate() * 100)) + "%)" + ".FBX";
			saveScene(tmp.c_str(), pManager, pScene, true);

			node->RemoveNodeAttribute(newMesh);
			node->SetNodeAttribute(pMesh);
			flag2 = false;
		}

		if ((int)(*Triangles).size() < threshold_3) {
			// After Reduction, Now Init Reduction VertexBuffer
			Init_Buffer(pMesh, model_3);

			FbxMesh *newMesh = CreateNewMesh(pControlPoints, pMesh, pScene);
			// Update the Node Atrribute
			node->RemoveNodeAttribute(pMesh);
			node->SetNodeAttribute(newMesh);
			// Export new Mesh
			std::string tmp = std::string(".\\LODs\\") + std::string("Save") + srcFbxName.substr(0, srcFbxName.size() - 4) + "_(" + std::to_string(int(model_3.GetReductionRate() * 100)) + "%)" + ".FBX";
			saveScene(tmp.c_str(), pManager, pScene, true);

			node->RemoveNodeAttribute(newMesh);
			node->SetNodeAttribute(pMesh);
			break;
		}
	}
}

void GenerateLOD::Init_Buffer(fbxsdk::FbxMesh *pMesh, Model &model)
{
	model.SetVertexCount(static_cast<int>(ControlP->size()));
	model.SetPolygonCount(static_cast<int>(Triangles->size()));

	// Create the VertexBuffer
	model.CreateVertextBuffer();

	IDirect3DVertexBuffer9 *g_pVB = model.GetVertexBuffer();

	CUSTOMVERTEX *pVertices;
	g_pVB->Lock(0, 0, (void **)&pVertices, 0);
	FbxLayerElementArrayTemplate<FbxVector2> *pUVarray = NULL;
	bool hasUV = pMesh->GetTextureUV(&pUVarray);
	if (!hasUV) {
		printf("Has No Textures!\n");
	}

	WORD count = 0;
	std::unordered_map<int, WORD> RemainPoints;
	FbxVector4 *pControlPoints = pMesh->GetControlPoints();
	for (std::unordered_map<int, Point>::iterator it = (*ControlP).begin(); it != (*ControlP).end(); ++it) {
		const FbxVector4 &P = pControlPoints[it->first];
		pVertices[count].x = static_cast<FLOAT>(P[0]);
		pVertices[count].y = static_cast<FLOAT>(P[1]);
		pVertices[count].z = static_cast<FLOAT>(P[2]);

		// uv coordinate
		pVertices[count].tu = static_cast<FLOAT>((pUVarray->GetAt(*it->second.uvSet.begin()))[0]);
		pVertices[count].tv = static_cast<FLOAT>((pUVarray->GetAt(*it->second.uvSet.begin()))[1]);

		RemainPoints[it->first] = count;
		++count;
	}
	g_pVB->Unlock();

	
	// Create the IndexBuffer
	model.CreateIndexBuffer();

	IDirect3DIndexBuffer9 *g_pIB = model.GetIndexBuffer();

	WORD *index;
	int cnt = 0;
	g_pIB->Lock(0, 0, (void **)&index, 0);
	for (std::unordered_map<int, Face>::iterator it = (*Triangles).begin(); it != (*Triangles).end(); ++it) {
		index[cnt++] = RemainPoints[it->second.points[0]];
		index[cnt++] = RemainPoints[it->second.points[1]];
		index[cnt++] = RemainPoints[it->second.points[2]];
	}
	g_pIB->Unlock();
}

void GenerateLOD::saveScene(const char *pFilename, FbxManager *pFbxManager, FbxScene *pFbxScene, bool pAsASCII = false)
{
	//Save the scene using the Python FBX API 
	FbxExporter *exporter = FbxExporter::Create(pFbxManager, "");
	bool isInitialized;
	if (pAsASCII) {
		// DEBUG : Initialize the FbxExporter object to export in ASCII.
		int asciiFormatIndex = getASCIIFormatIndex(pFbxManager);
		isInitialized = exporter->Initialize(pFilename, asciiFormatIndex);
	}
	else
		isInitialized = exporter->Initialize(pFilename);

	exporter->Export(pFbxScene);

	exporter->Destroy();
}

int GenerateLOD::getASCIIFormatIndex(FbxManager *pManager)
{
	//Obtain the index of the ASCII export format. '''
	//# Count the number of formats we can write to.
	int numFormats = pManager->GetIOPluginRegistry()->GetWriterFormatCount();

	//# Set the default format to the native binary format.
	int formatIndex = pManager->GetIOPluginRegistry()->GetNativeWriterFormat();

	//# Get the FBX format index whose corresponding description contains "ascii".
	for (int i = 0; i < numFormats; ++i) {
		//# First check if the writer is an FBX writer.
		if (pManager->GetIOPluginRegistry()->WriterIsFBX(i)) {
			//# Obtain the description of the FBX writer.
			const char *description = pManager->GetIOPluginRegistry()->GetWriterFormatDescription(i);
			//# Check if the description contains 'ascii'.
			if (strstr(description, "ascii") != NULL) {
				formatIndex = i;
				break;
			}
		}
	}
	//# Return the file format.
	return formatIndex;
}