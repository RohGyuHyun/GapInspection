// DialogAlarmMsg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "DialogAlarmMsg.h"
#include "afxdialogex.h"

#include "GapInspectionDlg.h"


// CDialogAlarmMsg ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CDialogAlarmMsg, CDialogEx)

CDialogAlarmMsg::CDialogAlarmMsg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDialogAlarmMsg::IDD, pParent)
{

}

CDialogAlarmMsg::~CDialogAlarmMsg()
{

}

void CDialogAlarmMsg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BTNENHCTRL_EDIT_ERROR_MSG, m_sMessage);
	DDX_Control(pDX, IDC_BTNENHCTRL_EDIT_ERROR_CODE, m_sCode);
}


BEGIN_MESSAGE_MAP(CDialogAlarmMsg, CDialogEx)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// CDialogAlarmMsg �޽��� ó�����Դϴ�.


BOOL CDialogAlarmMsg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	this->SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CDialogAlarmMsg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	if (bShow)
	{
		//m_btn_msg.SetCaption(m_sMessage);
		//MoveWindow(245, 280, 690, 450, TRUE);
	}
	else
	{

	}
}

BOOL CDialogAlarmMsg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
			return TRUE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CDialogAlarmMsg::SetMessage(char *code, char *msg)
{
	CString sData;

	sData.Format(_T("%S"), msg);
	m_sMessage.SetCaption(sData);	

	sData.Format(_T("%S"), code);
	m_sCode.SetCaption(sData);
}

BEGIN_EVENTSINK_MAP(CDialogAlarmMsg, CDialogEx)
	ON_EVENT(CDialogAlarmMsg, IDC_BTNENHCTRL_BUTTON_BUZZER_OFF, DISPID_CLICK, CDialogAlarmMsg::ClickBtnenhctrlButtonBuzzerOff, VTS_NONE)
	ON_EVENT(CDialogAlarmMsg, IDC_BTNENHCTRL_BUTTON_ALARM_CLOSE, DISPID_CLICK, CDialogAlarmMsg::ClickBtnenhctrlButtonAlarmClose, VTS_NONE)
END_EVENTSINK_MAP()


void CDialogAlarmMsg::ClickBtnenhctrlButtonBuzzerOff()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	CGapInspectionDlg * pdlg = (CGapInspectionDlg *)AfxGetApp()->m_pMainWnd;

	::SendMessage(pdlg->m_hWnd, USE_MSG_MOTION_TO_MAIN_DIALOG, 2, 0);
}


void CDialogAlarmMsg::ClickBtnenhctrlButtonAlarmClose()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	CGapInspectionDlg * pdlg = (CGapInspectionDlg *)AfxGetApp()->m_pMainWnd;

	::SendMessage(pdlg->m_hWnd, USE_MSG_MOTION_TO_MAIN_DIALOG, 1, 0);

	CDialog::OnOK();
}


