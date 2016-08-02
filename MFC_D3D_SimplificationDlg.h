
// MFC_D3D_SimplificationDlg.h : 头文件
//

#pragma once
#include "afxwin.h"


// CMFC_D3D_SimplificationDlg 对话框
class CMFC_D3D_SimplificationDlg : public CDialogEx
{
// 构造
public:
	CMFC_D3D_SimplificationDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFC_D3D_SIMPLIFICATION_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnKickIdle(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	CStatic m_3dview;
	afx_msg void OnBnClickedLod1();
	afx_msg void OnBnClickedLod2();
	afx_msg void OnBnClickedLod3();
	afx_msg void OnBnClickedSwitch();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedReset();
	afx_msg void OnBnClickedSavefbx();
};
