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
	// �����ӽ�
	m_right = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	m_up = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	m_look = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	m_position = D3DXVECTOR3(0.0f, 0.0f, -40.0f);

	// ��������ϵ��
	Scal = 1.0f;

	// ����
	SetWorldParams();
	SetViewParams();
	SetProjParams();
}

void Camera::SetWindow(int width, int height)
{
	window_width = width;
	window_height = height;
}

//������ m_position �� m_up ���½��� dist ��ƽ��
void Camera::Walk(float dist)
{
	m_position += dist * m_up;
}

//������ m_position �� m_right ���ҽ��� dist ��ƽ��
void Camera::Strafe(float dist)
{
	m_position += dist * m_right;
}

//����������������ת��Ҳ������X����ת�������ϼ� m_up ������ m_right ���� angle ����ת
//Pitch ʵ���ϼ��� X ����ת
void Camera::Pitch(float angle)
{
	D3DXMATRIX rotation;
	D3DXMatrixRotationAxis(&rotation, &m_right, angle);
	
	// �������ת������� m_up ����
	D3DXVec3TransformNormal(&m_up, &m_up, &rotation);

	//�������ת������� m_look ����
	D3DXVec3TransformNormal(&m_look, &m_look, &rotation);

	// ���㾭����ת�� m_position ��λ��
	D3DXVec3TransformCoord(&m_position, &m_position, &rotation);
}

//����������������ת��Ҳ������ Y ����ת�������ϼ����������� m_right, m_up, m_look �� Y ����ת angle �Ƕ�
void Camera::Yaw(float angle)
{
	D3DXMATRIX rotation;
	D3DXMatrixRotationAxis(&rotation, &m_up, angle);

	// �������ת������� m_right ����
	D3DXVec3TransformNormal(&m_right, &m_right, &rotation);

	// �������ת������� m_look ����
	D3DXVec3TransformNormal(&m_look, &m_look, &rotation);

	// ���㾭����ת�� m_position ��λ��
	D3DXVec3TransformCoord(&m_position, &m_position, &rotation);
}

// �������൱�� �������� D3DXMatrixLookAtLH(&view_matrix, &m_position, &m_LookAt, &m_up)
void Camera::UpdateView()
{
	D3DXVECTOR3 right;
	D3DXVECTOR3 up;
	D3DXVECTOR3 look;

	// �� m_up ��� m_look �õ� һ�����ҵ� right ����
	D3DXVec3Cross(&right, &m_up, &m_look);
	D3DXVec3Normalize(&right, &right);

	// �� m_look �����һ���õ��� right �õ� һ�����ҵ� right ����
	D3DXVec3Cross(&up, &m_look, &right);
	D3DXVec3Normalize(&up, &up);

	// �� m_look ��׼�� �õ� look ����
	D3DXVec3Normalize(&look, &m_look);

	float x = -D3DXVec3Dot(&m_position, &right);
	float y = -D3DXVec3Dot(&m_position, &up);
	float z = -D3DXVec3Dot(&m_position, &look);

	// �����º��ȫ���������µ�ԭ��������
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
			if (is_dragged) {     // ����Ҽ���ק��ת
				float dx = XMConvertToRadians(0.25f * (mouse_x - PreviousX));
				float dy = XMConvertToRadians(0.25f * (mouse_y - PreviousY));

				Pitch(dy); //��������� X ����ת����Ҫ���� y �����ϵĽǶ�����
				
				Yaw(dx); // ��������� Y ����ת����Ҫ���� x �����ϵĽǶ�����

				PreviousX = mouse_x; // ���� PreviousX �� PreviousY Ϊ��ǰ��Ļ����
				PreviousY = mouse_y;

				SetViewParams(); //�����º�� ��������Ӧ�õ� �ӽǾ�����
				//UpdateView();
			}

			else if (is_Left) {                  // ��������קƽ��
				float dx = -0.1f * (mouse_x - XTrans);
				float dy = 0.1f * (mouse_y - YTrans);

				Walk(dy); // �� ����ƽ��

				Strafe(dx); //�� ����ƽ��

				XTrans = mouse_x; // ���� XTrans �� XTranY Ϊ��ǰ��Ļ����
				YTrans = mouse_y;

				SetViewParams(); //�����º�� ��������Ӧ�õ� �ӽǾ�����
				//UpdateView();
			}
			break;
		}

		case WM_MOUSEWHEEL:
		{
			// �Ŵ�
			if (zDelta > 0) {
				Scal += 0.1f;
			}

			// ��С
			else {
				Scal -= 0.1f;
			}

			// �������ž���
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
	   �����ǹؼ��㣬����ֱ���� m_look ������Ϊ���� D3DXMatrixLookAtLH �ĵ���������
	   ��Ϊ���������������������� target �㣬�����ǳ��������
	   �����Ҫͨ�������λ�úͳ�����������������Ŀ�� target
	*/
	D3DXVECTOR3 m_LookAt = m_position + m_look;
	D3DXMatrixLookAtLH(&view_matrix, &m_position, &m_LookAt, &m_up);
}

void Camera::SetProjParams()
{
	D3DXMatrixPerspectiveFovLH(&proj_matrix, m_fovY, m_aspect, m_nearZ, m_farZ);
}