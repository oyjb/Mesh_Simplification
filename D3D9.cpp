#pragma once
#include "Common.h"

D3D9::D3D9()
{
	g_pD3D = NULL;
	Device = NULL;
	pTexture = NULL;

	has_Texture = false;

	m_Width = 0;
	m_Height = 0;
}

D3D9::~D3D9()
{
	Destroy();
}

bool D3D9::InitD3D9(int Width, int Height, HWND hwnd)
{
	if (NULL == (g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)))
		return false;

	D3DPRESENT_PARAMETERS D3DPP;
	ZeroMemory(&D3DPP, sizeof(D3DPP));

	D3DPP.Windowed = true;
	D3DPP.SwapEffect = D3DSWAPEFFECT_DISCARD;
	D3DPP.BackBufferFormat = D3DFMT_UNKNOWN;
	//D3DPP.EnableAutoDepthStencil = TRUE;
	D3DPP.AutoDepthStencilFormat = D3DFMT_D16;

	//Create the D3DDevice
	if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hwnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&D3DPP,
		&Device)))
	{
		return false;
	}
	// Turn off D3D lighting
	Device->SetRenderState(D3DRS_LIGHTING, FALSE);

	// Turn on the zbuffer
	Device->SetRenderState(D3DRS_ZENABLE, TRUE);

	// Turn on the Scalling
	Device->SetRenderState(D3DRS_POINTSCALEENABLE, TRUE);

	// Create the Texture From File
	D3DXCreateTextureFromFile(Device, "Picture_For_Texture.jpg", &pTexture);

	m_Width = Width;
	m_Height = Height;

	camera.SetWindow(Width, Height);

	return true;
}

IDirect3DDevice9* D3D9::GetDevice()
{
	return Device;
}

void D3D9::SetupMatrices()
{
	D3DXMATRIX matWorld = camera.GetWorldMatrix();
	Device->SetTransform(D3DTS_WORLD, &matWorld);

	D3DXMATRIX matView = camera.GetViewMatrix();
	Device->SetTransform(D3DTS_VIEW, &matView);

	D3DXMATRIX matProj = camera.GetProjMatrix();
	Device->SetTransform(D3DTS_PROJECTION, &matProj);
}

void D3D9::SetHasTexture(bool hasTexture)
{
	has_Texture = hasTexture;
}

bool D3D9::GetHasTexture()
{
	return has_Texture;
}

void D3D9::SetupTexture()
{
	D3DFVF_CUSTOMVERTEX = D3DFVF_XYZ | D3DFVF_TEX1;
	Device->SetTexture(0, pTexture);
	Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

	// 设置背面裁剪(逆时针裁剪)
	Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	// 设置纹理过滤器， 线性计算，效率相对较高
	Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
	Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC);

	// 设置mipfilter，基于最近等级的图案进行缩放
	Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

	// 设置纹理超过1时的处理方式
	Device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	Device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

	// 设置纹理状态
	Device->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
	Device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	Device->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);
	Device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	Device->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
}

void D3D9::DrawMesh()
{
	////////////////  Draw the left Scene  /////////////////////////////
	D3DVIEWPORT9 vp = { 0, 0, m_Width / 2, m_Height, 0, 1 };
	Device->SetViewport(&vp);
	Device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 128, 0), 1.0f, 0);

	Device->SetStreamSource(0, model_0.GetVertexBuffer(), 0, sizeof(CUSTOMVERTEX));
	Device->SetFVF(D3DFVF_CUSTOMVERTEX);
	Device->SetIndices(model_0.GetIndexBuffer());
	Device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, model_0.GetVertexCount(), 0, model_0.GetPolygonCount());
	//////////////////////////////////////////////////////////////////

	////////////////  Draw the right Scene  /////////////////////////////
	vp = { m_Width / 2, 0, m_Width / 2, m_Height, 0, 1 };
	Device->SetViewport(&vp);
	Device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 255), 1.0f, 0);

	Device->SetStreamSource(0, model.GetVertexBuffer(), 0, sizeof(CUSTOMVERTEX));
	Device->SetFVF(D3DFVF_CUSTOMVERTEX);
	Device->SetIndices(model.GetIndexBuffer());
	Device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, model.GetVertexCount(), 0, model.GetPolygonCount());
	////////////////////////////////////////////////////////////////////
}

void D3D9::Render()
{
	if (NULL == Device)
	{
		return;
	}

	//Sets up the world, view, and projection transform matrices.
	SetupMatrices();

	if (has_Texture) {
		SetupTexture();
	}
	else {
		D3DFVF_CUSTOMVERTEX = D3DFVF_XYZ;
		Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
		// 关闭背面裁剪
		Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		// 关闭纹理
		Device->SetTexture(0, 0);
	}

	if (SUCCEEDED(Device->BeginScene()))
	{
		DrawMesh();
		//End the Scene
		Device->EndScene();
	}
	RECT rectSour = { 0,  0, m_Width, m_Height };
	RECT rectDest = { 0,  0, m_Width, m_Height };
	Device->Present(&rectSour, &rectDest, NULL, NULL);
	//device->Present(NULL, NULL, NULL, NULL);
}

void D3D9::Destroy()
{
	if (NULL != Device)
		Device->Release();
	if (NULL != g_pD3D)
		g_pD3D->Release();
	if (NULL != pTexture)
		pTexture->Release();
}

Camera& D3D9::GetCamera()
{
	return camera;
}