#include <d3dx9.h>
#include "Common.h"

Model::Model()
{
	g_pVB = NULL;
	g_pIB = NULL;

	PolygonCount = 0;
	VertexCount = 0;
	ReductionRate = 0.0f;

	D3DFVF_CUSTOMVERTEX = D3DFVF_XYZ;
}

Model::~Model()
{
	if (NULL != g_pVB)
		g_pVB->Release();
	if (NULL != g_pIB)
		g_pIB->Release();
}

void Model::SetDevice(IDirect3DDevice9 *device)
{
	Device = device;
}

IDirect3DDevice9* Model::GetDevice()
{
	return Device;
}

void Model::SetPolygonCount(int polygonCount)
{
	PolygonCount = polygonCount;
}

void Model::SetVertexCount(int vertexCount)
{
	VertexCount = vertexCount;
}

int Model::GetPolygonCount()
{
	return PolygonCount;
}

int Model::GetVertexCount()
{
	return VertexCount;
}

void Model::CreateVertextBuffer()
{
	// Create the VertexBuffer
	Device->CreateVertexBuffer(VertexCount * sizeof(CUSTOMVERTEX),
		0, D3DFVF_CUSTOMVERTEX,
		D3DPOOL_DEFAULT, &g_pVB, NULL);
}

void Model::CreateIndexBuffer()
{
	// Create the IndexBuffer
	Device->CreateIndexBuffer(3 * PolygonCount * sizeof(WORD),
		D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &g_pIB, NULL);
}

IDirect3DVertexBuffer9* Model::GetVertexBuffer()
{
	return g_pVB;
}

IDirect3DIndexBuffer9*  Model::GetIndexBuffer()
{
	return g_pIB;
}

void Model::SetReductionRate(float rate)
{
	ReductionRate = rate;
}

float Model::GetReductionRate()
{
	return ReductionRate;
}