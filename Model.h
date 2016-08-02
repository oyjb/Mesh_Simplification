#pragma once
#include <d3dx9.h>

class Model {
public:
	Model();
	~Model();

	void SetDevice(IDirect3DDevice9 *device);
	IDirect3DDevice9 *GetDevice();

	void SetPolygonCount(int polygonCount);
	void SetVertexCount(int vertexCount);
	int GetPolygonCount();
	int GetVertexCount();

	void CreateVertextBuffer();
	void CreateIndexBuffer();
	IDirect3DVertexBuffer9* GetVertexBuffer();
	IDirect3DIndexBuffer9*  GetIndexBuffer();

	void SetReductionRate(float rate);
	float GetReductionRate();


private:
	IDirect3DDevice9        *Device;

	IDirect3DVertexBuffer9  *g_pVB;
	IDirect3DIndexBuffer9   *g_pIB;

	int PolygonCount;
	int VertexCount;

	float ReductionRate;
};