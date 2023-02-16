// DialogAlarmMsg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "DialogAlarmMsg.h"
#include "afxdialogex.h"

#include "GapInspectionDlg.h"


// CDialogAlarmMsg 대화 상자입니다.

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


// CDialogAlarmMsg 메시지 처리기입니다.


BOOL CDialogAlarmMsg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	this->SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CDialogAlarmMsg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
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
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
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
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CGapInspectionDlg * pdlg = (CGapInspectionDlg *)AfxGetApp()->m_pMainWnd;

	::SendMessage(pdlg->m_hWnd, USE_MSG_MOTION_TO_MAIN_DIALOG, 2, 0);
}


void CDialogAlarmMsg::ClickBtnenhctrlButtonAlarmClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CGapInspectionDlg * pdlg = (CGapInspectionDlg *)AfxGetApp()->m_pMainWnd;

	::SendMessage(pdlg->m_hWnd, USE_MSG_MOTION_TO_MAIN_DIALOG, 1, 0);

	CDialog::OnOK();
}


