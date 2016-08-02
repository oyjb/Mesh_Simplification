#pragma once
#include <d3dx9.h>
#include <xnamath.h>
#include "Camera.h"
#include <cmath>
#include <string>

using namespace std;

Camera::Camera() :
	m_right(1.0f, 0.0f, 0.0f),
	m_up(0.0f, 1.0f, 0.0f),
	m_look(0.0f, 0.0f, 1.0f),
	m_position(0.0f, 0.0f, -40.0f),

	m_aspect(800.0f / 600),
	m_fovY(3.1415926 * 0.25),
	m_nearZ(1.0f),
	m_farZ(1000.0f),

	is_dragged(false),
	is_Left(false),

	PreviousX(0),
	PreviousY(0),

	XTrans(0),
	YTrans(0),

	Scal(1.0f)
{
	SetWorldParams();
	SetViewParams();
	SetProjParams();
}

Camera::~Camera() { }

void Camera::Reset()
{
	// 重置视角
	m_right = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	m_up = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	m_look = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	m_position = D3DXVECTOR3(0.0f, 0.0f, -40.0f);

	// 重置缩放系数
	Scal = 1.0f;

	// 重置
	SetWorldParams();
	SetViewParams();
	SetProjParams();
}

void Camera::SetWindow(int width, int height)
{
	window_width = width;
	window_height = height;
}

//功能是 m_position 沿 m_up 上下进行 dist 的平移
void Camera::Walk(float dist)
{
	m_position += dist * m_up;
}

//功能是 m_position 沿 m_right 左右进行 dist 的平移
void Camera::Strafe(float dist)
{
	m_position += dist * m_right;
}

//功能是用于上下旋转，也就是绕X轴旋转，本质上即 m_up 向量绕 m_right 进行 angle 的旋转
//Pitch 实际上即绕 X 轴旋转
void Camera::Pitch(float angle)
{
	D3DXMATRIX rotation;
	D3DXMatrixRotationAxis(&rotation, &m_right, angle);
	
	// 用这个旋转矩阵更新 m_up 向量
	D3DXVec3TransformNormal(&m_up, &m_up, &rotation);

	//用这个旋转矩阵更新 m_look 向量
	D3DXVec3TransformNormal(&m_look, &m_look, &rotation);

	// 计算经过旋转后 m_position 的位置
	D3DXVec3TransformCoord(&m_position, &m_position, &rotation);
}

//功能是用于左右旋转，也就是绕 Y 轴旋转，本质上即三个坐标轴 m_right, m_up, m_look 绕 Y 轴旋转 angle 角度
void Camera::Yaw(float angle)
{
	D3DXMATRIX rotation;
	D3DXMatrixRotationAxis(&rotation, &m_up, angle);

	// 用这个旋转矩阵更新 m_right 向量
	D3DXVec3TransformNormal(&m_right, &m_right, &rotation);

	// 用这个旋转矩阵更新 m_look 向量
	D3DXVec3TransformNormal(&m_look, &m_look, &rotation);

	// 计算经过旋转后 m_position 的位置
	D3DXVec3TransformCoord(&m_position, &m_position, &rotation);
}

// 本函数相当于 辅助函数 D3DXMatrixLookAtLH(&view_matrix, &m_position, &m_LookAt, &m_up)
void Camera::UpdateView()
{
	D3DXVECTOR3 right;
	D3DXVECTOR3 up;
	D3DXVECTOR3 look;

	// 由 m_up 叉乘 m_look 得到 一个向右的 right 向量
	D3DXVec3Cross(&right, &m_up, &m_look);
	D3DXVec3Normalize(&right, &right);

	// 由 m_look 叉乘上一步得到的 right 得到 一个向右的 right 向量
	D3DXVec3Cross(&up, &m_look, &right);
	D3DXVec3Normalize(&up, &up);

	// 将 m_look 标准化 得到 look 向量
	D3DXVec3Normalize(&look, &m_look);

	float x = -D3DXVec3Dot(&m_position, &right);
	float y = -D3DXVec3Dot(&m_position, &up);
	float z = -D3DXVec3Dot(&m_position, &look);

	// 将更新后的全部向量更新到原来向量中
	m_right = right;
	m_up = up;
	m_look = look;

	view_matrix(0, 0) = m_right.x;    view_matrix(0, 1) = m_up.x;    view_matrix(0, 2) = m_look.x;    view_matrix(0, 3) = 0;
	view_matrix(1, 0) = m_right.y;    view_matrix(1, 1) = m_up.y;    view_matrix(1, 2) = m_look.y;    view_matrix(1, 3) = 0;
	view_matrix(2, 0) = m_right.z;    view_matrix(2, 1) = m_up.z;    view_matrix(2, 2) = m_look.z;    view_matrix(2, 3) = 0;
	view_matrix(3, 0) = x;            view_matrix(3, 1) = y;         view_matrix(3, 2) = z;           view_matrix(3, 3) = 1;
}

LRESULT Camera::HandleMassages(UINT uMsg, short zDelta, int mouse_x, int mouse_y)
{
	switch (uMsg)
	{
		case WM_RBUTTONDOWN:
		{
			if (mouse_x >= 0 && mouse_x < window_width &&
				mouse_y >= 0 && mouse_y < window_height) {
				is_dragged = true;
				PreviousX = mouse_x;
				PreviousY = mouse_y;
			}
			break;
		}
		case WM_RBUTTONUP:
		{
			is_dragged = false;
			break;
		}

		case WM_MOUSEMOVE:
		{
			if (is_dragged) {     // 鼠标右键拖拽旋转
				float dx = XMConvertToRadians(0.25f * (mouse_x - PreviousX));
				float dy = XMConvertToRadians(0.25f * (mouse_y - PreviousY));

				Pitch(dy); //鼠标上下绕 X 轴旋转，需要的是 y 方向上的角度增量
				
				Yaw(dx); // 鼠标左右绕 Y 轴旋转，需要的是 x 方向上的角度增量

				PreviousX = mouse_x; // 更新 PreviousX 和 PreviousY 为当前屏幕坐标
				PreviousY = mouse_y;

				SetViewParams(); //将更新后的 各个向量应用到 视角矩阵中
				//UpdateView();
			}

			else if (is_Left) {                  // 鼠标左键拖拽平移
				float dx = -0.1f * (mouse_x - XTrans);
				float dy = 0.1f * (mouse_y - YTrans);

				Walk(dy); // 沿 上下平移

				Strafe(dx); //沿 左右平移

				XTrans = mouse_x; // 更新 XTrans 和 XTranY 为当前屏幕坐标
				YTrans = mouse_y;

				SetViewParams(); //将更新后的 各个向量应用到 视角矩阵中
				//UpdateView();
			}
			break;
		}

		case WM_MOUSEWHEEL:
		{
			// 放大
			if (zDelta > 0) {
				Scal += 0.1f;
			}

			// 缩小
			else {
				Scal -= 0.1f;
			}

			// 设置缩放矩阵
			SetWorldParams();
			break;
		}
		case WM_LBUTTONDOWN:
		{
			if (mouse_x >= 0 && mouse_x < window_width &&
				mouse_y >= 0 && mouse_y < window_height) {
				is_Left = true;
				XTrans = mouse_x;
				YTrans = mouse_y;
			}
			break;
		}
		case WM_LBUTTONUP:
		{
			is_Left = false;
			break;
		}
		default:
			break;
	}
	return S_OK;
}

const D3DXMATRIX Camera::GetWorldMatrix() const
{
	return world_matrix;
}

const D3DXMATRIX Camera::GetViewMatrix() const
{
	return view_matrix;
}

const D3DXMATRIX Camera::GetProjMatrix() const
{
	return proj_matrix;
}

void Camera::SetWorldParams()
{
	D3DXMatrixScaling(&world_matrix, Scal, Scal, Scal);
}

void Camera::SetViewParams()
{
	/* 
	   这里是关键点，不能直接用 m_look 向量作为函数 D3DXMatrixLookAtLH 的第三个参数
	   因为第三个参数是照相机看向的 target 点，而不是朝向的向量
	   因此需要通过照相机位置和朝向向量计算出看向的目标 target
	*/
	D3DXVECTOR3 m_LookAt = m_position + m_look;
	D3DXMatrixLookAtLH(&view_matrix, &m_position, &m_LookAt, &m_up);
}

void Camera::SetProjParams()
{
	D3DXMatrixPerspectiveFovLH(&proj_matrix, m_fovY, m_aspect, m_nearZ, m_farZ);
}