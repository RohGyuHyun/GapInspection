// DialogDriveSelect.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "DialogDriveSelect.h"
#include "afxdialogex.h"


// CDialogDriveSelect 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDialogDriveSelect, CDialogEx)

CDialogDriveSelect::CDialogDriveSelect(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDialogDriveSelect::IDD, pParent)
{

}

CDialogDriveSelect::~CDialogDriveSelect()
{

}

void CDialogDriveSelect::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	int i = 0;
	DDX_Control(pDX, IDC_BTNENHCTRL_DRIVE_SELECT_0, m_cDriveSelect[i++]);
	DDX_Control(pDX, IDC_BTNENHCTRL_DRIVE_SELECT_1, m_cDriveSelect[i++]);
	DDX_Control(pDX, IDC_BTNENHCTRL_DRIVE_SELECT_2, m_cDriveSelect[i++]);
	DDX_Control(pDX, IDC_BTNENHCTRL_DRIVE_SELECT_3, m_cDriveSelect[i++]);
	DDX_Control(pDX, IDC_BTNENHCTRL_DRIVE_SELECT_4, m_cDriveSelect[i++]);
	DDX_Control(pDX, IDC_BTNENHCTRL_DRIVE_SELECT_5, m_cDriveSelect[i++]);
	DDX_Control(pDX, IDC_BTNENHCTRL_DRIVE_SELECT_6, m_cDriveSelect[i++]);
	DDX_Control(pDX, IDC_BTNENHCTRL_DRIVE_SELECT_7, m_cDriveSelect[i++]);
	DDX_Control(pDX, IDC_BTNENHCTRL_DRIVE_SELECT_8, m_cDriveSelect[i++]);
	DDX_Control(pDX, IDC_BTNENHCTRL_DRIVE_SELECT_9, m_cDriveSelect[i++]);
}


BEGIN_MESSAGE_MAP(CDialogDriveSelect, CDialogEx)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// CDialogDriveSelect 메시지 처리기입니다.
BEGIN_EVENTSINK_MAP(CDialogDriveSelect, CDialogEx)
	ON_EVENT(CDialogDriveSelect, IDC_BTNENHCTRL_DRIVE_SELECT_0, DISPID_CLICK, CDialogDriveSelect::DriveSelect, VTS_NONE)
	ON_EVENT(CDialogDriveSelect, IDC_BTNENHCTRL_DRIVE_SELECT_1, DISPID_CLICK, CDialogDriveSelect::DriveSelect, VTS_NONE)
	ON_EVENT(CDialogDriveSelect, IDC_BTNENHCTRL_DRIVE_SELECT_2, DISPID_CLICK, CDialogDriveSelect::DriveSelect, VTS_NONE)
	ON_EVENT(CDialogDriveSelect, IDC_BTNENHCTRL_DRIVE_SELECT_3, DISPID_CLICK, CDialogDriveSelect::DriveSelect, VTS_NONE)
	ON_EVENT(CDialogDriveSelect, IDC_BTNENHCTRL_DRIVE_SELECT_4, DISPID_CLICK, CDialogDriveSelect::DriveSelect, VTS_NONE)
	ON_EVENT(CDialogDriveSelect, IDC_BTNENHCTRL_DRIVE_SELECT_5, DISPID_CLICK, CDialogDriveSelect::DriveSelect, VTS_NONE)
	ON_EVENT(CDialogDriveSelect, IDC_BTNENHCTRL_DRIVE_SELECT_6, DISPID_CLICK, CDialogDriveSelect::DriveSelect, VTS_NONE)
	ON_EVENT(CDialogDriveSelect, IDC_BTNENHCTRL_DRIVE_SELECT_7, DISPID_CLICK, CDialogDriveSelect::DriveSelect, VTS_NONE)
	ON_EVENT(CDialogDriveSelect, IDC_BTNENHCTRL_DRIVE_SELECT_8, DISPID_CLICK, CDialogDriveSelect::DriveSelect, VTS_NONE)
	ON_EVENT(CDialogDriveSelect, IDC_BTNENHCTRL_DRIVE_SELECT_9, DISPID_CLICK, CDialogDriveSelect::DriveSelect, VTS_NONE)
END_EVENTSINK_MAP()


void CDialogDriveSelect::DriveSelect()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CString strDrive;
	CBtnenhctrl *pBtnEnh = (CBtnenhctrl*)GetFocus();

	pBtnEnh->GetWindowTextW(strDrive);
	m_strDriveName.Format(_T("%s"), strDrive.Left(1));

	this->ShowWindow(SW_HIDE);
}


BOOL CDialogDriveSelect::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


BOOL CDialogDriveSelect::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if( pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE )
	{
		return TRUE;
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}


void CDialogDriveSelect::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if(bShow)
	{
		TCHAR buffer[256] = {0, };
		CString strDriveName;
		int count = GetLogicalDriveStrings(256, buffer);
		int nDriveCnt = count / 4;
		strDriveName.Format(_T("Empty"));
		for(int i = 0; i < MAX_DRIVE_SELECT; i++)
		{
			if(i < nDriveCnt)
			{
				m_cDriveSelect[i].SetEnabled(TRUE);
			}
			else
			{
				m_cDriveSelect[i].SetEnabled(FALSE);
			}
			m_cDriveSelect[i].SetWindowTextW(strDriveName);
		}

		for(int i = 0; i < count / 4; i++) 
		{
			strDriveName.Format(_T("%S:\\"), buffer + i * 4);
			m_cDriveSelect[i].SetWindowTextW(strDriveName);
		}

		for(int i = 0; i < nDriveCnt; i++)
		{
			m_cDriveSelect[i].GetWindowTextW(strDriveName);
			if(!strDriveName.Left(1).Compare(m_strDriveName))
			{
				m_cDriveSelect[i].SetValue(TRUE);
			}
		}
	}
	else
	{

	}
}
