// DialogDriveSelect.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "DialogAlgorithmSelect.h"
#include "afxdialogex.h"


// CDialogDriveSelect ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CDialogAlgorithmSelect, CDialogEx)

CDialogAlgorithmSelect::CDialogAlgorithmSelect(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDialogAlgorithmSelect::IDD, pParent)
{

}

CDialogAlgorithmSelect::~CDialogAlgorithmSelect()
{

}

void CDialogAlgorithmSelect::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	int i = 0;
	DDX_Control(pDX, IDC_BTNENHCTRL_ALGORITHM_SELECT_0, m_cAlgorithmSelect[i++]);
	DDX_Control(pDX, IDC_BTNENHCTRL_ALGORITHM_SELECT_1, m_cAlgorithmSelect[i++]);
	DDX_Control(pDX, IDC_BTNENHCTRL_ALGORITHM_SELECT_2, m_cAlgorithmSelect[i++]);
}


BEGIN_MESSAGE_MAP(CDialogAlgorithmSelect, CDialogEx)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// CDialogDriveSelect �޽��� ó�����Դϴ�.
BEGIN_EVENTSINK_MAP(CDialogAlgorithmSelect, CDialogEx)
	ON_EVENT(CDialogAlgorithmSelect, IDC_BTNENHCTRL_ALGORITHM_SELECT_0, DISPID_CLICK, CDialogAlgorithmSelect::AlgorithmSelect, VTS_NONE)
	ON_EVENT(CDialogAlgorithmSelect, IDC_BTNENHCTRL_ALGORITHM_SELECT_1, DISPID_CLICK, CDialogAlgorithmSelect::AlgorithmSelect, VTS_NONE)
	ON_EVENT(CDialogAlgorithmSelect, IDC_BTNENHCTRL_ALGORITHM_SELECT_2, DISPID_CLICK, CDialogAlgorithmSelect::AlgorithmSelect, VTS_NONE)
END_EVENTSINK_MAP()


void CDialogAlgorithmSelect::AlgorithmSelect()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	CBtnenhctrl *pBtnEnh = (CBtnenhctrl*)GetFocus();
	
	pBtnEnh->GetWindowTextW(m_strAlgorithmName);

	switch(pBtnEnh->GetDlgCtrlID())
	{
	case IDC_BTNENHCTRL_ALGORITHM_SELECT_0:
		m_nAlgorithmIndex = 0;
		break;
	case IDC_BTNENHCTRL_ALGORITHM_SELECT_1:
		m_nAlgorithmIndex = 1;
		break;
	case IDC_BTNENHCTRL_ALGORITHM_SELECT_2:
		m_nAlgorithmIndex = 2;
		break;
	}

	this->ShowWindow(SW_HIDE);
}

BOOL CDialogAlgorithmSelect::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}


BOOL CDialogAlgorithmSelect::PreTranslateMessage(MSG* pMsg)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	if( pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE )
	{
		return TRUE;
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

void CDialogAlgorithmSelect::SetAlgorithm(int nAlgorithmIdx)
{
	m_nAlgorithmIndex = nAlgorithmIdx;
	if(m_nAlgorithmIndex == 0)
	{
		m_strAlgorithmName.Format(_T("Winner"));
	}
	else if(m_nAlgorithmIndex == 1)
	{
		m_strAlgorithmName.Format(_T("Bloom"));
	}
	else
	{
		m_strAlgorithmName.Format(_T("Empty"));
	}
}

void CDialogAlgorithmSelect::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	if(bShow)
	{
		m_cAlgorithmSelect[m_nAlgorithmIndex].SetValue(TRUE);
	}
	else
	{

	}
}
