#pragma once
#include <d3dx9.h>
#include "Camera.h"

class D3D9
{
public:
	D3D9();
	~D3D9();

public:
	bool InitD3D9(int Width, int Height, HWND hwnd);
	IDirect3DDevice9 *GetDevice();

	void SetupMatrices();
	void SetHasTexture(bool hasTexture);
	bool GetHasTexture();
	void SetupTexture();
	void DrawMesh();
	void Render();
	void Destroy();

	Camera& GetCamera();

private:
	IDirect3D9              *g_pD3D;
	IDirect3DDevice9        *Device;
	IDirect3DTexture9       *pTexture;

	bool has_Texture;

	DWORD m_Width;
	DWORD m_Height;

private:
	Camera  camera;
};