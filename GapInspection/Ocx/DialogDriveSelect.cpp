// DialogDriveSelect.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "DialogDriveSelect.h"
#include "afxdialogex.h"


// CDialogDriveSelect ��ȭ �����Դϴ�.

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


// CDialogDriveSelect �޽��� ó�����Դϴ�.
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
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	CString strDrive;
	CBtnenhctrl *pBtnEnh = (CBtnenhctrl*)GetFocus();

	pBtnEnh->GetWindowTextW(strDrive);
	m_strDriveName.Format(_T("%s"), strDrive.Left(1));

	this->ShowWindow(SW_HIDE);
}


BOOL CDialogDriveSelect::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}


BOOL CDialogDriveSelect::PreTranslateMessage(MSG* pMsg)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	if( pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE )
	{
		return TRUE;
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}


void CDialogDriveSelect::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
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
