
// MFC_D3D_Simplification.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CMFC_D3D_SimplificationApp: 
// �йش����ʵ�֣������ MFC_D3D_Simplification.cpp
//

class CMFC_D3D_SimplificationApp : public CWinApp
{
public:
	CMFC_D3D_SimplificationApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CMFC_D3D_SimplificationApp theApp;