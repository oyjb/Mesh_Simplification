
// MFC_D3D_SimplificationDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"


// CMFC_D3D_SimplificationDlg �Ի���
class CMFC_D3D_SimplificationDlg : public CDialogEx
{
// ����
public:
	CMFC_D3D_SimplificationDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFC_D3D_SIMPLIFICATION_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
