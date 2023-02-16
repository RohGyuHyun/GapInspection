// DialogInspPoint.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "GapInspection.h"
#include "DialogInspPoint.h"
#include "afxdialogex.h"


// CDialogInspPoint ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CDialogInspPoint, CDialogEx)

CDialogInspPoint::CDialogInspPoint(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDialogInspPoint::IDD, pParent)
{

}

CDialogInspPoint::~CDialogInspPoint()
{
}

void CDialogInspPoint::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	int i = 0;
	DDX_Control(pDX, IDC_BTNENHCTRL_SPEC_INSP_POINT_0, m_InspPoint[i++]);
	DDX_Control(pDX, IDC_BTNENHCTRL_SPEC_INSP_POINT_1, m_InspPoint[i++]);
	DDX_Control(pDX, IDC_BTNENHCTRL_SPEC_INSP_POINT_2, m_InspPoint[i++]);
	DDX_Control(pDX, IDC_BTNENHCTRL_SPEC_INSP_POINT_3, m_InspPoint[i++]);
	DDX_Control(pDX, IDC_BTNENHCTRL_SPEC_INSP_POINT_4, m_InspPoint[i++]);
	DDX_Control(pDX, IDC_BTNENHCTRL_SPEC_INSP_POINT_5, m_InspPoint[i++]);

	i = 0;
	DDX_Control(pDX, IDC_BTNENHCTRL_SPEC_INSP_POINT_6, m_InspPoint2[i++]);
	DDX_Control(pDX, IDC_BTNENHCTRL_SPEC_INSP_POINT_7, m_InspPoint2[i++]);
	DDX_Control(pDX, IDC_BTNENHCTRL_SPEC_INSP_POINT_8, m_InspPoint2[i++]);
	DDX_Control(pDX, IDC_BTNENHCTRL_SPEC_INSP_POINT_9, m_InspPoint2[i++]);
	DDX_Control(pDX, IDC_BTNENHCTRL_SPEC_INSP_POINT_10, m_InspPoint2[i++]);
	DDX_Control(pDX, IDC_BTNENHCTRL_SPEC_INSP_POINT_11, m_InspPoint2[i++]);

}


BEGIN_MESSAGE_MAP(CDialogInspPoint, CDialogEx)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// CDialogInspPoint �޽��� ó�����Դϴ�.


void CDialogInspPoint::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
}


BOOL CDialogInspPoint::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	InitInspPoint();

	/*GetDlgItem(IDC_BTNENHCTRL_SPEC_INSP_POINT_6)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_BTNENHCTRL_SPEC_INSP_POINT_7)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_BTNENHCTRL_SPEC_INSP_POINT_8)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_BTNENHCTRL_SPEC_INSP_POINT_9)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_BTNENHCTRL_SPEC_INSP_POINT_10)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_BTNENHCTRL_SPEC_INSP_POINT_11)->ShowWindow(SW_HIDE);*/

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}


BOOL CDialogInspPoint::PreTranslateMessage(MSG* pMsg)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	if( pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE )
	{
		return TRUE;
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

void CDialogInspPoint::SetInspRsltPointView(int nInspPoint, int nRslt)
{
	InspRsltPointView(nInspPoint, nRslt);
}

void CDialogInspPoint::InspRsltPointView(int nInspPoint, int nRslt)
{
	if(nRslt == TRUE)
	{
		m_InspPoint2[nInspPoint].SetBackColor(RGB(0, 200, 0));
		m_InspPoint[nInspPoint].SetBackColor(RGB(0, 200, 0));
	}
	else if(nRslt == FALSE)
	{
		m_InspPoint2[nInspPoint].SetBackColor(RGB(200, 0, 0));
		m_InspPoint[nInspPoint].SetBackColor(RGB(200, 0, 0));
	}
	else
	{
		m_InspPoint2[nInspPoint].SetBackColor(RGB(150, 150, 150));
	}
}

void CDialogInspPoint::InitInspPoint()
{
	for(int i = 0; i < MAX_INSP_POINT_CNT; i++)
	{
		m_InspPoint[i].SetBackColor(RGB(150, 150, 150));
		m_InspPoint2[i].SetBackColor(RGB(150, 150, 150));
	}
}

void CDialogInspPoint::SetEachRsltPoint(int nInspPoint, int nRslt)
{
	if(nRslt == TRUE)
	{
		m_InspPoint[nInspPoint].SetBackColor(RGB(0, 200, 0));
	}
	else if(nRslt == FALSE)
	{
		m_InspPoint[nInspPoint].SetBackColor(RGB(255, 255, 0));
	}
	else if(nRslt == 2)
	{

	}
	else
	{
		m_InspPoint[nInspPoint].SetBackColor(RGB(150, 150, 150));
	}
}