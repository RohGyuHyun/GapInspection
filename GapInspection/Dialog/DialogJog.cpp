// DialogJog.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "GapInspection.h"
#include "DialogJog.h"
#include "afxdialogex.h"


// CDialogJog ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CDialogJog, CDialogEx)

CDialogJog::CDialogJog(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDialogJog::IDD, pParent)
{
	for(int i = 0; i < MAX_MOTION_CNT; i++)
	{
		m_dCurPos[i] = 0;
		m_dStepPitch[i] = 1;
	}
}

CDialogJog::~CDialogJog()
{
	if(this->IsWindowVisible())
	{
		KillTimer(JOG_TIMER_UPDATE_CUR_POS);
	}
}

void CDialogJog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BTNENHCTRL_TEACH_X_STEP, m_ctrJogStep[0]);
	DDX_Control(pDX, IDC_BTNENHCTRL_TEACH_Y_STEP, m_ctrJogStep[1]);
	DDX_Control(pDX, IDC_BTNENHCTRL_TEACH_Z_STEP, m_ctrJogStep[2]);
	DDX_Control(pDX, IDC_BTNENHCTRL_TEACH_X_CUR_POS, m_ctrJogCurPos[0]);
	DDX_Control(pDX, IDC_BTNENHCTRL_TEACH_Y_CUR_POS, m_ctrJogCurPos[1]);
	DDX_Control(pDX, IDC_BTNENHCTRL_TEACH_Z_CUR_POS, m_ctrJogCurPos[2]);
}


BEGIN_MESSAGE_MAP(CDialogJog, CDialogEx)
	ON_WM_TIMER()
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// CDialogJog �޽��� ó�����Դϴ�.


void CDialogJog::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	int nEvent = (int)nIDEvent;

	switch(nEvent)
	{
	case JOG_TIMER_UPDATE_CUR_POS:
		if(this->IsWindowVisible())
			UpdateCurPos();
		break;
	default:

		break;
	}

	CDialogEx::OnTimer(nIDEvent);
}


void CDialogJog::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	if(bShow)
	{
		SetTimer(JOG_TIMER_UPDATE_CUR_POS, 100, NULL);
	}
	else
	{
		KillTimer(JOG_TIMER_UPDATE_CUR_POS);
	}

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
}

BOOL CDialogJog::PreTranslateMessage(MSG* pMsg)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	if( pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE )
	{
		return TRUE;
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}
BEGIN_EVENTSINK_MAP(CDialogJog, CDialogEx)
	ON_EVENT(CDialogJog, IDC_BTNENHCTRL_TEACH_X_JOG_UP, DISPID_CLICK, CDialogJog::JogMove, VTS_NONE)
	ON_EVENT(CDialogJog, IDC_BTNENHCTRL_TEACH_X_JOG_DOWN, DISPID_CLICK, CDialogJog::JogMove, VTS_NONE)
	ON_EVENT(CDialogJog, IDC_BTNENHCTRL_TEACH_Y_JOG_UP, DISPID_CLICK, CDialogJog::JogMove, VTS_NONE)
	ON_EVENT(CDialogJog, IDC_BTNENHCTRL_TEACH_Y_JOG_DOWN, DISPID_CLICK, CDialogJog::JogMove, VTS_NONE)
	ON_EVENT(CDialogJog, IDC_BTNENHCTRL_TEACH_Z_JOG_UP, DISPID_CLICK, CDialogJog::JogMove, VTS_NONE)
	ON_EVENT(CDialogJog, IDC_BTNENHCTRL_TEACH_Z_JOG_DOWN, DISPID_CLICK, CDialogJog::JogMove, VTS_NONE)
	ON_EVENT(CDialogJog, IDC_BTNENHCTRL_TEACH_JOG_SET_CUR_POS, DISPID_CLICK, CDialogJog::ClickBtnenhctrlTeachJogSetCurPos, VTS_NONE)
	ON_EVENT(CDialogJog, IDC_BTNENHCTRL_TEACH_JOG_SET_CUR_POS2, DISPID_CLICK, CDialogJog::ClickBtnenhctrlTeachJogSetCurPos, VTS_NONE)
	ON_EVENT(CDialogJog, IDC_BTNENHCTRL_TEACH_X_STEP, DISPID_CLICK, CDialogJog::ClickBtnenhctrlTeachStep, VTS_NONE)
	ON_EVENT(CDialogJog, IDC_BTNENHCTRL_TEACH_Y_STEP, DISPID_CLICK, CDialogJog::ClickBtnenhctrlTeachStep, VTS_NONE)
	ON_EVENT(CDialogJog, IDC_BTNENHCTRL_TEACH_Z_STEP, DISPID_CLICK, CDialogJog::ClickBtnenhctrlTeachStep, VTS_NONE)
END_EVENTSINK_MAP()




void CDialogJog::JogMove()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	int nDialogID;
	CString strText;
	CBtnenhctrl *pBtnEnh = (CBtnenhctrl*)GetFocus();
	nDialogID = pBtnEnh->GetDlgCtrlID();

	switch(nDialogID)
	{
	case IDC_BTNENHCTRL_TEACH_X_JOG_UP:
		m_bPlusMinuse = TRUE;
		AfxGetMainWnd()->SendMessage(USE_MSG_JOG_TO_MAIN_DIALOG, JOG_TO_MAIN_AXIS_X_JOG_MOVE, DIRECTION_UP);
		break;
	case IDC_BTNENHCTRL_TEACH_Y_JOG_UP:
		m_bPlusMinuse = TRUE;
		AfxGetMainWnd()->SendMessage(USE_MSG_JOG_TO_MAIN_DIALOG, JOG_TO_MAIN_AXIS_Y_JOG_MOVE, DIRECTION_UP);
		break;
	case IDC_BTNENHCTRL_TEACH_Z_JOG_UP:
		m_bPlusMinuse = TRUE;
		AfxGetMainWnd()->SendMessage(USE_MSG_JOG_TO_MAIN_DIALOG, JOG_TO_MAIN_AXIS_Z_JOG_MOVE, DIRECTION_UP);
		break;
	case IDC_BTNENHCTRL_TEACH_X_JOG_DOWN:
		m_bPlusMinuse = FALSE;
		AfxGetMainWnd()->SendMessage(USE_MSG_JOG_TO_MAIN_DIALOG, JOG_TO_MAIN_AXIS_X_JOG_MOVE, DIRECTION_DW);
		break;
	case IDC_BTNENHCTRL_TEACH_Y_JOG_DOWN:
		m_bPlusMinuse = FALSE;
		AfxGetMainWnd()->SendMessage(USE_MSG_JOG_TO_MAIN_DIALOG, JOG_TO_MAIN_AXIS_Y_JOG_MOVE, DIRECTION_DW);
		break;
	case IDC_BTNENHCTRL_TEACH_Z_JOG_DOWN:
		m_bPlusMinuse = FALSE;
		AfxGetMainWnd()->SendMessage(USE_MSG_JOG_TO_MAIN_DIALOG, JOG_TO_MAIN_AXIS_Z_JOG_MOVE, DIRECTION_DW);
		break;
	default:

		break;
	}
}

void CDialogJog::UpdateCurPos()
{
	double dCurPos[MAX_MOTION_CNT] = {0.,};
	CString strText;

	for(int i = 0; i < MAX_MOTION_CNT; i++)
	{
		//Current Pos Get
		dCurPos[i] = m_dCurPos[i];

		strText.Format(_T("%0.3f"), dCurPos[i]);
		m_ctrJogCurPos[i].SetWindowTextW(strText);
	}
}

void CDialogJog::ClickBtnenhctrlTeachJogSetCurPos()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	CBtnenhctrl *pBtnEnh = (CBtnenhctrl*)GetFocus();
	switch(pBtnEnh->GetDlgCtrlID())
	{
	case IDC_BTNENHCTRL_TEACH_JOG_SET_CUR_POS:
		AfxGetMainWnd()->SendMessage(USE_MSG_JOG_TO_MAIN_DIALOG, JOG_TO_MAIN_SET_CUR_POS_SEQ, 0);
		break;
	case IDC_BTNENHCTRL_TEACH_JOG_SET_CUR_POS2:
		AfxGetMainWnd()->SendMessage(USE_MSG_JOG_TO_MAIN_DIALOG, JOG_TO_MAIN_SET_CUR_POS_SEQ, 1);
		break;
	}
}


void CDialogJog::ClickBtnenhctrlTeachStep()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	int nDialogID;
	CString strText;
	CBtnenhctrl *pBtnEnh = (CBtnenhctrl*)GetFocus();
	nDialogID = pBtnEnh->GetDlgCtrlID();
	CGetNumDlg dlg;
	pBtnEnh->GetWindowTextW(strText);
	dlg.SetstrNum(strText);

	if(dlg.DoModal() != IDOK) return;

	strText = dlg.GetstrNum();

	switch(nDialogID)
	{
	case IDC_BTNENHCTRL_TEACH_X_STEP:
		m_dStepPitch[0] = _wtof(strText);
		strText.Empty();
		strText.Format(_T("%0.3f"), m_dStepPitch[0]);
		break;
	case IDC_BTNENHCTRL_TEACH_Y_STEP:
		m_dStepPitch[1] = _wtof(strText);
		strText.Empty();
		strText.Format(_T("%0.3f"), m_dStepPitch[1]);
		break;
	case IDC_BTNENHCTRL_TEACH_Z_STEP:
		m_dStepPitch[2] = _wtof(strText);
		strText.Empty();
		strText.Format(_T("%0.3f"), m_dStepPitch[2]);
		break;
	}

	pBtnEnh->SetWindowTextW(strText);
}


BOOL CDialogJog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

	CRect wndRect;

	SystemParametersInfo(SPI_GETWORKAREA, NULL, &wndRect, NULL);

	//int nShow = this->ShowWindow(SW_SHOW);
	int nWidth = wndRect.right - wndRect.left;
	int nHeight = wndRect.bottom - wndRect.top;

	this->SetWindowPos(&wndTopMost, (wndRect.right - nWidth), (wndRect.bottom - nHeight), 0, 0, SWP_NOSIZE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}
