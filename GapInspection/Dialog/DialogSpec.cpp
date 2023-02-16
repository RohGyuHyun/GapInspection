// DialogSpec.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "GapInspection.h"
#include "DialogSpec.h"
#include "afxdialogex.h"


// CDialogSpec 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDialogSpec, CDialogEx)

CDialogSpec::CDialogSpec(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDialogSpec::IDD, pParent)
{
	m_nSpecToSpreadIdx = 0;
}

CDialogSpec::~CDialogSpec()
{
	if(m_pdlgPointOffset != NULL)
		delete m_pdlgPointOffset;
}

void CDialogSpec::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	int i = 0;
	DDX_Control(pDX, IDC_BTNENHCTRL_SPEC_MIN_VAL_0, m_ctrMinSpec[i++]);
	DDX_Control(pDX, IDC_BTNENHCTRL_SPEC_MIN_VAL_1, m_ctrMinSpec[i++]);
	DDX_Control(pDX, IDC_BTNENHCTRL_SPEC_MIN_VAL_2, m_ctrMinSpec[i++]);
	//DDX_Control(pDX, IDC_BTNENHCTRL_SPEC_MIN_VAL_3, m_ctrMinSpec[i++]);
	//DDX_Control(pDX, IDC_BTNENHCTRL_SPEC_MIN_VAL_4, m_ctrMinSpec[i++]);
	//DDX_Control(pDX, IDC_BTNENHCTRL_SPEC_MIN_VAL_5, m_ctrMinSpec[i++]);
	i = 0;
	DDX_Control(pDX, IDC_BTNENHCTRL_SPEC_MAX_VAL_0, m_ctrMaxSpec[i++]);
	DDX_Control(pDX, IDC_BTNENHCTRL_SPEC_MAX_VAL_1, m_ctrMaxSpec[i++]);
	DDX_Control(pDX, IDC_BTNENHCTRL_SPEC_MAX_VAL_2, m_ctrMaxSpec[i++]);
	//DDX_Control(pDX, IDC_BTNENHCTRL_SPEC_MAX_VAL_3, m_ctrMaxSpec[i++]);
	//DDX_Control(pDX, IDC_BTNENHCTRL_SPEC_MAX_VAL_4, m_ctrMaxSpec[i++]);
	//DDX_Control(pDX, IDC_BTNENHCTRL_SPEC_MAX_VAL_5, m_ctrMaxSpec[i++]);
}


BEGIN_MESSAGE_MAP(CDialogSpec, CDialogEx)
	ON_WM_SHOWWINDOW()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CDialogSpec 메시지 처리기입니다.
BEGIN_EVENTSINK_MAP(CDialogSpec, CDialogEx)
	ON_EVENT(CDialogSpec, IDC_BTNENHCTRL_SPEC_MIN_VAL_0, DISPID_CLICK, CDialogSpec::SpecInput, VTS_NONE)
	ON_EVENT(CDialogSpec, IDC_BTNENHCTRL_SPEC_MIN_VAL_1, DISPID_CLICK, CDialogSpec::SpecInput, VTS_NONE)
	ON_EVENT(CDialogSpec, IDC_BTNENHCTRL_SPEC_MIN_VAL_2, DISPID_CLICK, CDialogSpec::SpecInput, VTS_NONE)
	//ON_EVENT(CDialogSpec, IDC_BTNENHCTRL_SPEC_MIN_VAL_3, DISPID_CLICK, CDialogSpec::SpecInput, VTS_NONE)
	//ON_EVENT(CDialogSpec, IDC_BTNENHCTRL_SPEC_MIN_VAL_4, DISPID_CLICK, CDialogSpec::SpecInput, VTS_NONE)
	//ON_EVENT(CDialogSpec, IDC_BTNENHCTRL_SPEC_MIN_VAL_5, DISPID_CLICK, CDialogSpec::SpecInput, VTS_NONE)
	ON_EVENT(CDialogSpec, IDC_BTNENHCTRL_SPEC_MAX_VAL_0, DISPID_CLICK, CDialogSpec::SpecInput, VTS_NONE)
	ON_EVENT(CDialogSpec, IDC_BTNENHCTRL_SPEC_MAX_VAL_1, DISPID_CLICK, CDialogSpec::SpecInput, VTS_NONE)
	ON_EVENT(CDialogSpec, IDC_BTNENHCTRL_SPEC_MAX_VAL_2, DISPID_CLICK, CDialogSpec::SpecInput, VTS_NONE)
	//ON_EVENT(CDialogSpec, IDC_BTNENHCTRL_SPEC_MAX_VAL_3, DISPID_CLICK, CDialogSpec::SpecInput, VTS_NONE)
	//ON_EVENT(CDialogSpec, IDC_BTNENHCTRL_SPEC_MAX_VAL_4, DISPID_CLICK, CDialogSpec::SpecInput, VTS_NONE)
	//ON_EVENT(CDialogSpec, IDC_BTNENHCTRL_SPEC_MAX_VAL_5, DISPID_CLICK, CDialogSpec::SpecInput, VTS_NONE)
	ON_EVENT(CDialogSpec, IDC_BTNENHCTRL_SPEC_APPLY, DISPID_CLICK, CDialogSpec::ClickBtnenhctrlSpecApply, VTS_NONE)
	ON_EVENT(CDialogSpec, IDC_BTNENHCTRL_SPEC_CANCEL, DISPID_CLICK, CDialogSpec::ClickBtnenhctrlSpecCancel, VTS_NONE)
	ON_EVENT(CDialogSpec, IDC_BTNENHCTRL_SPEC_TO_SPREAD_0, DISPID_CLICK, CDialogSpec::ClickBtnenhctrlSpecToSpread0, VTS_NONE)
	ON_EVENT(CDialogSpec, IDC_BTNENHCTRL_SPEC_TO_SPREAD_1, DISPID_CLICK, CDialogSpec::ClickBtnenhctrlSpecToSpread1, VTS_NONE)
	ON_EVENT(CDialogSpec, IDC_BTNENHCTRL_SPEC_OFFSET, DISPID_CLICK, CDialogSpec::ClickBtnenhctrlSpecOffset, VTS_NONE)
END_EVENTSINK_MAP()


void CDialogSpec::SpecInput()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
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

	if(dlg.DoModal() != IDOK) return;

	strText = dlg.GetstrNum();

	nDialogID = pBtnEnh->GetDlgCtrlID();
	if(nDialogID >= IDC_BTNENHCTRL_SPEC_MAX_VAL_0)
	{
		nInspIdx = nDialogID - IDC_BTNENHCTRL_SPEC_MAX_VAL_0;
		m_ctrMinSpec[nInspIdx].GetWindowTextW(strTemp);
		dTempVal = _wtof(strTemp);
		if(dTempVal >= _wtof(strText))
		{
			if(m_nSpecToSpreadIdx == 0)
			{
				strTemp.Format(_T("Max Val = %0.3f <= Min Val = %0.3f Fail !!"), _wtof(strText), dTempVal);
				AfxMessageBox(strTemp);
				return;
			}
		}
	}
	else
	{
		nInspIdx = nDialogID - IDC_BTNENHCTRL_SPEC_MIN_VAL_0;
		m_ctrMaxSpec[nInspIdx].GetWindowTextW(strTemp);
		dTempVal = _wtof(strTemp);
		if(dTempVal <= _wtof(strText))
		{
			if(m_nSpecToSpreadIdx == 0)
			{
				strTemp.Format(_T("Max Val = %0.3f <= Min Val = %0.3f Fail !!"), dTempVal, _wtof(strText));
				AfxMessageBox(strTemp);
				return;
			}
		}
	}

	if(_ttof(strText) > 150. || _ttof(strText) < -150.) 
	{
		strTemp.Format(_T("Min Spec Val = -150.000 <= Input Val = %0.3f <= Max Spec Val = 150.000 "), _wtof(strText));
		AfxMessageBox(strTemp);
		return;
	}

	pBtnEnh->SetWindowTextW(strText);
}

void CDialogSpec::UpdatePara(BOOL isUpdate, int nSpecToSpreadIdx)
{
	CString strText;
	if(isUpdate)
	{
		UpdateData(TRUE);

		if(nSpecToSpreadIdx == 0)
		{
			for(int i = 0; i < MAX_RSLT_POINT_CNT; i++)
			{
				m_ctrMinSpec[i].GetWindowTextW(strText);
				m_Spec[i].dMinSpec = _wtof(strText);
			}

			for(int i = 0; i < MAX_RSLT_POINT_CNT; i++)
			{
				m_ctrMaxSpec[i].GetWindowTextW(strText);
				m_Spec[i].dMaxSpec = _wtof(strText);
			}
		}
		else
		{
			for(int i = 0; i < MAX_RSLT_POINT_CNT; i++)
			{
				m_ctrMinSpec[i].GetWindowTextW(strText);
				m_Spread[i].dMinSpec = _wtof(strText);
			}

			for(int i = 0; i < MAX_RSLT_POINT_CNT; i++)
			{
				m_ctrMaxSpec[i].GetWindowTextW(strText);
				m_Spread[i].dMaxSpec = _wtof(strText);
			}
		}
	}
	else
	{
		if(nSpecToSpreadIdx == 0)
		{
			for(int i = 0; i < MAX_RSLT_POINT_CNT; i++)
			{
				strText.Format(_T("%0.3f"), m_Spec[i].dMinSpec);
				m_ctrMinSpec[i].SetCaption(strText);
			}

			for(int i = 0; i < MAX_RSLT_POINT_CNT; i++)
			{
				strText.Format(_T("%0.3f"), m_Spec[i].dMaxSpec);
				m_ctrMaxSpec[i].SetCaption(strText);
			}
		}
		else
		{
			for(int i = 0; i < MAX_RSLT_POINT_CNT; i++)
			{
				strText.Format(_T("%0.3f"), m_Spread[i].dMinSpec);
				m_ctrMinSpec[i].SetCaption(strText);
			}

			for(int i = 0; i < MAX_RSLT_POINT_CNT; i++)
			{
				strText.Format(_T("%0.3f"), m_Spread[i].dMaxSpec);
				m_ctrMaxSpec[i].SetCaption(strText);
			}
		}

		UpdateData(FALSE);
	}
}

BOOL CDialogSpec::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	m_pdlgPointOffset = new CDialogSpecPointOffset();
	m_pdlgPointOffset->Create(IDD_DIALOG_SPEC_POINT_OFFSET, this);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CDialogSpec::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);
	if(bShow)
	{
		m_nSpecToSpreadIdx = 0;
		CBtnenhctrl *pBtnEnh = (CBtnenhctrl*)GetDlgItem(IDC_BTNENHCTRL_SPEC_TO_SPREAD_0);
		pBtnEnh->SetValue(TRUE);
		UpdatePara(FALSE, m_nSpecToSpreadIdx);
		m_isSave = FALSE;
	}
	else
	{
		if(m_pdlgPointOffset->IsWindowVisible())
			m_pdlgPointOffset->ShowWindow(SW_HIDE);
	}
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}


BOOL CDialogSpec::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if( pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE )
	{
		return TRUE;
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}


void CDialogSpec::ClickBtnenhctrlSpecApply()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_isSave = TRUE;
	UpdatePara(TRUE, m_nSpecToSpreadIdx);
	ShowWindow(SW_HIDE);
}


void CDialogSpec::ClickBtnenhctrlSpecCancel()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	this->ShowWindow(SW_HIDE);
}

void CDialogSpec::GetPointOffset(TypeSpecPara *Spec)
{
	for(int i = 0; i < MAX_INSP_POINT_CNT; i++)
	{
		Spec[i] = m_PointOffset[i];
	}
}

void CDialogSpec::SetPointOffset(TypeSpecPara *Spec)
{
	for(int i = 0; i < MAX_INSP_POINT_CNT; i++)
	{
		m_PointOffset[i] = Spec[i];
	}
}

void CDialogSpec::GetSpec(TypeSpecPara *Spec)
{
	for(int i = 0; i < MAX_RSLT_POINT_CNT; i++)
	{
		Spec[i] = m_Spec[i];
	}
}

void CDialogSpec::SetSpec(TypeSpecPara *Spec)
{
	for(int i = 0; i < MAX_RSLT_POINT_CNT; i++)
	{
		m_Spec[i] = Spec[i];
	}
}

void CDialogSpec::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	int nEvent = (int)nIDEvent;

	switch(nEvent)
	{
	case 100:
		if(!m_pdlgPointOffset->IsWindowVisible())
		{
			KillTimer(100);
			for(int i = 0; i < MAX_INSP_POINT_CNT; i++)
			{
				m_PointOffset[i].dOffset = m_pdlgPointOffset->GetOffset(i).dOffset;
			}
		}
		break;
	}

	CDialogEx::OnTimer(nIDEvent);
}

//20190424 ngh
void CDialogSpec::GetSpread(TypeSpecPara *Spread)
{
	for(int i = 0; i < MAX_RSLT_POINT_CNT; i++)
	{
		Spread[i] = m_Spread[i];
	}
}

void CDialogSpec::SetSpread(TypeSpecPara *Spread)
{
	for(int i = 0; i < MAX_RSLT_POINT_CNT; i++)
	{
		m_Spread[i] = Spread[i];
	}
}

void CDialogSpec::ClickBtnenhctrlSpecToSpread0()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_nSpecToSpreadIdx = 0;
	UpdatePara(FALSE, m_nSpecToSpreadIdx);
}


void CDialogSpec::ClickBtnenhctrlSpecToSpread1()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_nSpecToSpreadIdx = 1;
	UpdatePara(FALSE, m_nSpecToSpreadIdx);
}


void CDialogSpec::ClickBtnenhctrlSpecOffset()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CRect rect;
	this->GetWindowRect(&rect);

	for(int i = 0; i < MAX_INSP_POINT_CNT; i++)
		m_pdlgPointOffset->SetOffset(i, m_PointOffset[i].dOffset);

	m_pdlgPointOffset->SetWindowMoveRect(rect);
	m_pdlgPointOffset->ShowWindow(SW_SHOW);
	SetTimer(100, 100, NULL);
}
