#include "stdafx.h"
#include "MFC_D3D_Simplification.h"
#include "MFC_D3D_SimplificationDlg.h"
#include "afxdialogex.h"
#include <string>
#include "Common.h"
#include "GenerateLOD.h"
#include "D3D9.h"
#include "Camera.h"
#include "Model.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

std::string srcFbxName; // Resouce Fbx File Name
DWORD D3DFVF_CUSTOMVERTEX = D3DFVF_XYZ;
Model model_0;  //  Original Model
Model model_1;  //  LOD1
Model model_2;  //  LOD2
Model model_3;  //  LOD3
D3D9  d3d9;      // d3d9
GenerateLOD generateLOD;  // Generate LOD
Model model;



class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};
CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}
void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()
// CMFC_D3D_SimplificationDlg �Ի���
CMFC_D3D_SimplificationDlg::CMFC_D3D_SimplificationDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MFC_D3D_SIMPLIFICATION_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFC_D3D_SimplificationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_3DVIEW, m_3dview);
}

BEGIN_MESSAGE_MAP(CMFC_D3D_SimplificationDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_LOD1, &CMFC_D3D_SimplificationDlg::OnBnClickedLod1)
	ON_BN_CLICKED(IDC_LOD2, &CMFC_D3D_SimplificationDlg::OnBnClickedLod2)
	ON_BN_CLICKED(IDC_LOD3, &CMFC_D3D_SimplificationDlg::OnBnClickedLod3)
	ON_BN_CLICKED(IDC_SWITCH, &CMFC_D3D_SimplificationDlg::OnBnClickedSwitch)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEWHEEL()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_KICKIDLE, OnKickIdle)
	ON_BN_CLICKED(IDC_BUTTON5, &CMFC_D3D_SimplificationDlg::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_RESET, &CMFC_D3D_SimplificationDlg::OnBnClickedReset)
	ON_BN_CLICKED(IDC_SAVEFBX, &CMFC_D3D_SimplificationDlg::OnBnClickedSavefbx)
END_MESSAGE_MAP()


// CMFC_D3D_SimplificationDlg ��Ϣ�������

BOOL CMFC_D3D_SimplificationDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	// ���ñ༭���ʼ����
	GetDlgItem(IDC_FILE_NAME)->SetWindowTextA("BunnyTexture.FBX");
	GetDlgItem(IDC_LOD1_RATE)->SetWindowTextA("50");
	GetDlgItem(IDC_LOD2_RATE)->SetWindowTextA("10");
	GetDlgItem(IDC_LOD3_RATE)->SetWindowTextA("1");

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	CRect rect;
	GetWindowRect(&rect);
	m_3dview.GetWindowRect(&rect);

	//��ʼ��ͼ�δ���
	HWND hWnd;
	hWnd = m_3dview.m_hWnd;

	d3d9.InitD3D9(rect.Width(), rect.Height(), hWnd);

	model_0.SetReductionRate(1.0f);
	model_1.SetReductionRate(0.5f);
	model_2.SetReductionRate(0.1f);
	model_3.SetReductionRate(0.01f);

	model_0.SetDevice(d3d9.GetDevice());
	model_1.SetDevice(d3d9.GetDevice());
	model_2.SetDevice(d3d9.GetDevice());
	model_3.SetDevice(d3d9.GetDevice());

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CMFC_D3D_SimplificationDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

LRESULT CMFC_D3D_SimplificationDlg::OnKickIdle(WPARAM wParam, LPARAM lParam)
{
	d3d9.Render();
	return 1;
}


// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CMFC_D3D_SimplificationDlg::OnPaint()
{
	d3d9.Render();
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CMFC_D3D_SimplificationDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CMFC_D3D_SimplificationDlg::OnBnClickedLod1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	// Set Current LOD
	std::string tmp = "Current LOD : LOD_1 # ";
	tmp += std::to_string(int(100 * model_1.GetReductionRate()));
	tmp += "%";
	GetDlgItem(IDC_INDICATE)->SetWindowTextA(tmp.c_str());

	// TODO: �ڴ���ӿؼ�֪ͨ����������
	// Get the Reduction rate
	CString str;
	GetDlgItem(IDC_LOD1_RATE)->GetWindowText(str);
	std::string rate(str.GetBuffer());

	int n = std::stoi(rate);
	if (!(rate.size() > 0 && n > 0 && n < 100)) {
		MessageBox("No LOD_1 !");
		return;
	}
	model = model_1;
	d3d9.Render();
}


void CMFC_D3D_SimplificationDlg::OnBnClickedLod2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	// Set Current LOD
	std::string tmp = "Current LOD : LOD_2 # ";
	tmp += std::to_string(int(100 * model_2.GetReductionRate()));
	tmp += "%";
	GetDlgItem(IDC_INDICATE)->SetWindowTextA(tmp.c_str());

	// TODO: �ڴ���ӿؼ�֪ͨ����������
	// Get the Reduction rate
	CString str;
	GetDlgItem(IDC_LOD2_RATE)->GetWindowText(str);
	std::string rate(str.GetBuffer());

	int n = std::stoi(rate);
	if (!(n > 0 && n < 100)) {
		MessageBox("No LOD_2 !");
		return;
	}
	model = model_2;
	d3d9.Render();
}


void CMFC_D3D_SimplificationDlg::OnBnClickedLod3()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	// Set Current LOD
	std::string tmp = "Current LOD : LOD_3 # ";
	tmp += std::to_string(int(100 * model_3.GetReductionRate()));
	tmp += "%";
	GetDlgItem(IDC_INDICATE)->SetWindowTextA(tmp.c_str());

	// TODO: �ڴ���ӿؼ�֪ͨ����������
	// Get the Reduction rate
	CString str;
	GetDlgItem(IDC_LOD3_RATE)->GetWindowText(str);
	std::string rate(str.GetBuffer());

	int n = std::stoi(rate);
	if (!(n > 0 && n < 100)) {
		MessageBox("No LOD_3 !");
		return;
	}
	model = model_3;
	d3d9.Render();
}


void CMFC_D3D_SimplificationDlg::OnBnClickedSwitch()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (d3d9.GetHasTexture())
		d3d9.SetHasTexture(false);
	else
		d3d9.SetHasTexture(true);
	d3d9.Render();
}


void CMFC_D3D_SimplificationDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	d3d9.GetCamera().HandleMassages(WM_LBUTTONDOWN, 0, point.x, point.y);
	CDialogEx::OnLButtonDown(nFlags, point);
}


void CMFC_D3D_SimplificationDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	d3d9.GetCamera().HandleMassages(WM_LBUTTONUP, 0, point.x, point.y);
	CDialogEx::OnLButtonUp(nFlags, point);
}


void CMFC_D3D_SimplificationDlg::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	d3d9.GetCamera().HandleMassages(WM_RBUTTONDOWN, 0, point.x, point.y);
	CDialogEx::OnRButtonDown(nFlags, point);
}


void CMFC_D3D_SimplificationDlg::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	d3d9.GetCamera().HandleMassages(WM_RBUTTONUP, 0, point.x, point.y);
	CDialogEx::OnRButtonUp(nFlags, point);
}


BOOL CMFC_D3D_SimplificationDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	d3d9.GetCamera().HandleMassages(WM_MOUSEWHEEL, zDelta, pt.x, pt.y);
	return CDialogEx::OnMouseWheel(nFlags, zDelta, pt);
}


void CMFC_D3D_SimplificationDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	d3d9.GetCamera().HandleMassages(WM_MOUSEMOVE, 0, point.x, point.y);
	CDialogEx::OnMouseMove(nFlags, point);
}


void CMFC_D3D_SimplificationDlg::OnBnClickedButton5()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString str;
	GetDlgItem(IDC_FILE_NAME)->GetWindowText(str);
	std::string filename(str.GetBuffer());
	str.ReleaseBuffer();

	GetDlgItem(IDC_LOD1_RATE)->GetWindowText(str);
	std::string rate1(str.GetBuffer());
	str.ReleaseBuffer();

	GetDlgItem(IDC_LOD2_RATE)->GetWindowText(str);
	std::string rate2(str.GetBuffer());
	str.ReleaseBuffer();

	GetDlgItem(IDC_LOD3_RATE)->GetWindowText(str);
	std::string rate3(str.GetBuffer());
	str.ReleaseBuffer();

	if (0 == filename.size()) {
		MessageBox("Please Input FBX File Name !");
		return;
	}
	if (rate1.size() == 0 || rate2.size() == 0 || rate3.size() == 0) {
		MessageBox("Please Input at least one LOD Rate!");
		return;
	}
	int n1 = std::stoi(rate1);
	int n2 = std::stoi(rate2);
	int n3 = std::stoi(rate3);

	if (n1 <= 0 || n2 <= 0 || n3 <= 0 || n1 >= 100 || n2 >= 100 || n3 >= 100) {
		MessageBox("LOD Rate illegal !");
		return;
	}
	if (filename == srcFbxName && n1 == int(model_1.GetReductionRate() * 100) &&
		n2 == int(model_2.GetReductionRate() * 100) &&
		n3 == int(model_3.GetReductionRate() * 100)) {
		MessageBox("Keep Same ! You may input different LODRates or FBX File Name.");
		return;
	}
	model_1.SetReductionRate((float)n1 / 100);
	model_2.SetReductionRate((float)n2 / 100);
	model_3.SetReductionRate((float)n3 / 100);

	srcFbxName = filename;

	// �ͷ�Ŀǰ����LOD�Ķ��㻺�����������
	if (model_1.GetVertexBuffer() != NULL) model_1.GetVertexBuffer()->Release();
	if (model_1.GetIndexBuffer() != NULL)  model_1.GetIndexBuffer()->Release();
	if (model_2.GetVertexBuffer() != NULL) model_2.GetVertexBuffer()->Release();
	if (model_2.GetIndexBuffer() != NULL)  model_2.GetIndexBuffer()->Release();
	if (model_3.GetVertexBuffer() != NULL) model_3.GetVertexBuffer()->Release();
	if (model_3.GetIndexBuffer() != NULL)  model_3.GetIndexBuffer()->Release();

	generateLOD.LoadFbx();
	d3d9.Render();

	// Set Current LOD
	std::string tmp = "Current LOD : LOD_1 # ";
	tmp += std::to_string(int(100 * model_1.GetReductionRate()));
	tmp += "%";
	GetDlgItem(IDC_INDICATE)->SetWindowTextA(tmp.c_str());
}


void CMFC_D3D_SimplificationDlg::OnBnClickedReset()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	d3d9.GetCamera().Reset();
	return;
}


void CMFC_D3D_SimplificationDlg::OnBnClickedSavefbx()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	MessageBox("SaveFBX Succeed!\nThe File Name Is : \"Save\" + Original FBX FileName + Rate");
	return;
}
