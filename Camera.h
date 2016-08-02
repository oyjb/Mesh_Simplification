#pragma once
#include <d3dx9.h>

class Camera
{
public:
	Camera();
	~Camera();

	void Reset();

public:
	LRESULT HandleMassages(UINT uMsg, short zDelta, int mouse_x, int mouse_y);

	void SetWorldParams();
	void SetViewParams();
	void SetProjParams();

	void SetWindow(int width, int height);

	//获得相关矩阵
	const D3DXMATRIX GetWorldMatrix() const;
	const D3DXMATRIX GetViewMatrix() const;
	const D3DXMATRIX GetProjMatrix() const;

	void SetPosition(float x, float y, float z) { m_position = D3DXVECTOR3(x, y, z); }
	D3DXVECTOR3 GetPosition() const { return m_position; }
	D3DXVECTOR3 GetRight() const { return m_right; }
	D3DXVECTOR3 GetUp() const { return m_up; }
	D3DXVECTOR3 GetLook() const { return m_look; }

	//获得投影相关参数
	float GetNearZ()    const { return m_nearZ; }
	float GetFarZ()     const { return m_farZ; }
	float GetFovY()     const { return m_fovY; }
	float GetFovX()     const { return atan(m_aspect * tan(m_fovY * 0.5f)) * 2.f; }
	float GetAspect()   const { return m_aspect; }

	//设置投影相关参数
	void SetLens(float fovY, float ratioAspect, float nearZ, float farZ)
	{
		m_fovY = fovY;
		m_aspect = ratioAspect;
		m_nearZ = nearZ;
		m_farZ = farZ;
	}

	//基本操作
	void Walk(float dist);          //前后行走
	void Strafe(float dist);        //左右平移
	void Pitch(float angle);        //上下点头
	void Yaw(float angle);      //左右插头

	//更新相关矩阵
	void UpdateView();


private:
	D3DXVECTOR3 m_right; //位置及三个坐标轴参数
	D3DXVECTOR3 m_up;
	D3DXVECTOR3 m_look;
	D3DXVECTOR3 m_position;

	float	m_aspect;  //投影相关系数
	float	m_fovY;
	float	m_nearZ;
	float	m_farZ;

	D3DXMATRIX world_matrix;
	D3DXMATRIX view_matrix;
	D3DXMATRIX proj_matrix;

	float Scal; //缩放系数

	int XTrans;
	int YTrans;

	int PreviousX;
	int PreviousY;

	bool is_dragged;
	bool is_Left;

	int window_width;
	int window_height;
};