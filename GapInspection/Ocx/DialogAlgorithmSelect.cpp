// DialogDriveSelect.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "DialogAlgorithmSelect.h"
#include "afxdialogex.h"


// CDialogDriveSelect 대화 상자입니다.

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


// CDialogDriveSelect 메시지 처리기입니다.
BEGIN_EVENTSINK_MAP(CDialogAlgorithmSelect, CDialogEx)
	ON_EVENT(CDialogAlgorithmSelect, IDC_BTNENHCTRL_ALGORITHM_SELECT_0, DISPID_CLICK, CDialogAlgorithmSelect::AlgorithmSelect, VTS_NONE)
	ON_EVENT(CDialogAlgorithmSelect, IDC_BTNENHCTRL_ALGORITHM_SELECT_1, DISPID_CLICK, CDialogAlgorithmSelect::AlgorithmSelect, VTS_NONE)
	ON_EVENT(CDialogAlgorithmSelect, IDC_BTNENHCTRL_ALGORITHM_SELECT_2, DISPID_CLICK, CDialogAlgorithmSelect::AlgorithmSelect, VTS_NONE)
END_EVENTSINK_MAP()


void CDialogAlgorithmSelect::AlgorithmSelect()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
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

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


BOOL CDialogAlgorithmSelect::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
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

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if(bShow)
	{
		m_cAlgorithmSelect[m_nAlgorithmIndex].SetValue(TRUE);
	}
	else
	{

	}
}
