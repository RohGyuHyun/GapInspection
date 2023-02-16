// Dialog\DialogSpecPointOffset.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "GapInspection.h"
#include "Dialog\DialogSpecPointOffset.h"
#include "afxdialogex.h"


// CDialogSpecPointOffset ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CDialogSpecPointOffset, CDialogEx)

CDialogSpecPointOffset::CDialogSpecPointOffset(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDialogSpecPointOffset::IDD, pParent)
{

}

CDialogSpecPointOffset::~CDialogSpecPointOffset()
{
}

void CDialogSpecPointOffset::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	int i = 0;
	DDX_Control(pDX, IDC_BTNENHCTRL_SPEC_OFFSET_MIN_VAL_6, m_ctrlOffset[i++]);
	DDX_Control(pDX, IDC_BTNENHCTRL_SPEC_OFFSET_MIN_VAL_7, m_ctrlOffset[i++]);
	DDX_Control(pDX, IDC_BTNENHCTRL_SPEC_OFFSET_MIN_VAL_8, m_ctrlOffset[i++]);
	DDX_Control(pDX, IDC_BTNENHCTRL_SPEC_OFFSET_MIN_VAL_9, m_ctrlOffset[i++]);
	DDX_Control(pDX, IDC_BTNENHCTRL_SPEC_OFFSET_MIN_VAL_10, m_ctrlOffset[i++]);
	DDX_Control(pDX, IDC_BTNENHCTRL_SPEC_OFFSET_MIN_VAL_11, m_ctrlOffset[i++]);
	
}


BEGIN_MESSAGE_MAP(CDialogSpecPointOffset, CDialogEx)
	ON_WM_SHOWWINDOW()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CDialogSpecPointOffset �޽��� ó�����Դϴ�.


void CDialogSpecPointOffset::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.

	if(bShow)
	{
		SetTimer(100, 100, NULL);
		UpdatePara(FALSE);
	}
	else
	{

	}
}

void CDialogSpecPointOffset::UpdatePara(BOOL isUpdate)
{
	CString strText;
	if(isUpdate)
	{
		UpdateData(TRUE);
		for(int i = 0; i < MAX_INSP_POINT_CNT; i++)
		{
			m_ctrlOffset[i].GetWindowTextW(strText);
			m_Spec[i].dOffset = _wtof(strText);
		}
	}
	else
	{
		for(int i = 0; i < MAX_INSP_POINT_CNT; i++)
		{
			strText.Format(_T("%f"), m_Spec[i].dOffset);
			m_ctrlOffset[i].SetWindowTextW(strText);
		}
		UpdateData(FALSE);
	}
}


BOOL CDialogSpecPointOffset::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}


BOOL CDialogSpecPointOffset::PreTranslateMessage(MSG* pMsg)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	return CDialogEx::PreTranslateMessage(pMsg);
}

TypeSpecPara CDialogSpecPointOffset::GetOffset(int nPoint)
{
	return m_Spec[nPoint];
}

void CDialogSpecPointOffset::SetOffset(int nPoint, double dOffset)
{
	m_Spec[nPoint].dOffset = dOffset;
}


BEGIN_EVENTSINK_MAP(CDialogSpecPointOffset, CDialogEx)
	ON_EVENT(CDialogSpecPointOffset, IDC_BTNENHCTRL_SPEC_OFFSET_APPLY, DISPID_CLICK, CDialogSpecPointOffset::ClickBtnenhctrlSpecApply, VTS_NONE)
	ON_EVENT(CDialogSpecPointOffset, IDC_BTNENHCTRL_SPEC_OFFSET_CANCEL, DISPID_CLICK, CDialogSpecPointOffset::ClickBtnenhctrlSpecCancel, VTS_NONE)
	ON_EVENT(CDialogSpecPointOffset, IDC_BTNENHCTRL_SPEC_OFFSET_MIN_VAL_6, DISPID_CLICK, CDialogSpecPointOffset::SpecInput, VTS_NONE)
	ON_EVENT(CDialogSpecPointOffset, IDC_BTNENHCTRL_SPEC_OFFSET_MIN_VAL_7, DISPID_CLICK, CDialogSpecPointOffset::SpecInput, VTS_NONE)
	ON_EVENT(CDialogSpecPointOffset, IDC_BTNENHCTRL_SPEC_OFFSET_MIN_VAL_8, DISPID_CLICK, CDialogSpecPointOffset::SpecInput, VTS_NONE)
	ON_EVENT(CDialogSpecPointOffset, IDC_BTNENHCTRL_SPEC_OFFSET_MIN_VAL_9, DISPID_CLICK, CDialogSpecPointOffset::SpecInput, VTS_NONE)
	ON_EVENT(CDialogSpecPointOffset, IDC_BTNENHCTRL_SPEC_OFFSET_MIN_VAL_10, DISPID_CLICK, CDialogSpecPointOffset::SpecInput, VTS_NONE)
	ON_EVENT(CDialogSpecPointOffset, IDC_BTNENHCTRL_SPEC_OFFSET_MIN_VAL_11, DISPID_CLICK, CDialogSpecPointOffset::SpecInput, VTS_NONE)
END_EVENTSINK_MAP()


void CDialogSpecPointOffset::ClickBtnenhctrlSpecApply()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	UpdatePara(TRUE);
	ClickBtnenhctrlSpecCancel();
}


void CDialogSpecPointOffset::ClickBtnenhctrlSpecCancel()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	ShowWindow(SW_HIDE);
}

void CDialogSpecPointOffset::SpecInput()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	double dVal;
	int nDialogID;
	double dTempVal = 0.;
	int nInspIdx = 0;
	CGetNumDlg dlg;
	CString strText, strTemp;
	CBtnenhctrl *pBtnEnh = (CBtnenhctrl*)GetFocus();
	pBtnEnh->GetWindowTextW(strText);
	dVal = _wtof(strText);

	dlg.SetstrNum(dVal);

	if(dlg.DoModal() != IDOK) 
		return;

	strText = dlg.GetstrNum();
	pBtnEnh->SetWindowTextW(strText);
}


void CDialogSpecPointOffset::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

	switch((int)nIDEvent)
	{
	case 100:
		if(this->IsWindowVisible())
		{
			KillTimer(100);
			this->MoveWindow(m_Rect.left, m_Rect.top, m_Rect.Width(), m_Rect.Height());
		}
		break;
	}

	CDialogEx::OnTimer(nIDEvent);
}
