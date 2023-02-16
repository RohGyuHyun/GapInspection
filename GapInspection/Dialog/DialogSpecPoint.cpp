// DialogSpec.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "GapInspection.h"
#include "DialogSpecPoint.h"
#include "afxdialogex.h"


// CDialogSpec 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDialogSpecPoint, CDialogEx)

CDialogSpecPoint::CDialogSpecPoint(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDialogSpecPoint::IDD, pParent)
{
	m_nSpecToSpreadIdx = 0;
}

CDialogSpecPoint::~CDialogSpecPoint()
{

}

void CDialogSpecPoint::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	int i = 0;
	DDX_Control(pDX, IDC_BTNENHCTRL_SPEC_MIN_VAL_6, m_ctrMinSpec[i++]);
	DDX_Control(pDX, IDC_BTNENHCTRL_SPEC_MIN_VAL_7, m_ctrMinSpec[i++]);
	DDX_Control(pDX, IDC_BTNENHCTRL_SPEC_MIN_VAL_8, m_ctrMinSpec[i++]);
	DDX_Control(pDX, IDC_BTNENHCTRL_SPEC_MIN_VAL_9, m_ctrMinSpec[i++]);
	DDX_Control(pDX, IDC_BTNENHCTRL_SPEC_MIN_VAL_10, m_ctrMinSpec[i++]);
	DDX_Control(pDX, IDC_BTNENHCTRL_SPEC_MIN_VAL_11, m_ctrMinSpec[i++]);
	i = 0;
	DDX_Control(pDX, IDC_BTNENHCTRL_SPEC_MAX_VAL_6, m_ctrMaxSpec[i++]);
	DDX_Control(pDX, IDC_BTNENHCTRL_SPEC_MAX_VAL_7, m_ctrMaxSpec[i++]);
	DDX_Control(pDX, IDC_BTNENHCTRL_SPEC_MAX_VAL_8, m_ctrMaxSpec[i++]);
	DDX_Control(pDX, IDC_BTNENHCTRL_SPEC_MAX_VAL_9, m_ctrMaxSpec[i++]);
	DDX_Control(pDX, IDC_BTNENHCTRL_SPEC_MAX_VAL_10, m_ctrMaxSpec[i++]);
	DDX_Control(pDX, IDC_BTNENHCTRL_SPEC_MAX_VAL_11, m_ctrMaxSpec[i++]);
}


BEGIN_MESSAGE_MAP(CDialogSpecPoint, CDialogEx)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// CDialogSpec 메시지 처리기입니다.
BEGIN_EVENTSINK_MAP(CDialogSpecPoint, CDialogEx)
	ON_EVENT(CDialogSpecPoint, IDC_BTNENHCTRL_SPEC_MIN_VAL_6, DISPID_CLICK, CDialogSpecPoint::SpecInput, VTS_NONE)
	ON_EVENT(CDialogSpecPoint, IDC_BTNENHCTRL_SPEC_MIN_VAL_7, DISPID_CLICK, CDialogSpecPoint::SpecInput, VTS_NONE)
	ON_EVENT(CDialogSpecPoint, IDC_BTNENHCTRL_SPEC_MIN_VAL_8, DISPID_CLICK, CDialogSpecPoint::SpecInput, VTS_NONE)
	ON_EVENT(CDialogSpecPoint, IDC_BTNENHCTRL_SPEC_MIN_VAL_9, DISPID_CLICK, CDialogSpecPoint::SpecInput, VTS_NONE)
	ON_EVENT(CDialogSpecPoint, IDC_BTNENHCTRL_SPEC_MIN_VAL_10, DISPID_CLICK, CDialogSpecPoint::SpecInput, VTS_NONE)
	ON_EVENT(CDialogSpecPoint, IDC_BTNENHCTRL_SPEC_MIN_VAL_11, DISPID_CLICK, CDialogSpecPoint::SpecInput, VTS_NONE)
	ON_EVENT(CDialogSpecPoint, IDC_BTNENHCTRL_SPEC_MAX_VAL_6, DISPID_CLICK, CDialogSpecPoint::SpecInput, VTS_NONE)
	ON_EVENT(CDialogSpecPoint, IDC_BTNENHCTRL_SPEC_MAX_VAL_7, DISPID_CLICK, CDialogSpecPoint::SpecInput, VTS_NONE)
	ON_EVENT(CDialogSpecPoint, IDC_BTNENHCTRL_SPEC_MAX_VAL_8, DISPID_CLICK, CDialogSpecPoint::SpecInput, VTS_NONE)
	ON_EVENT(CDialogSpecPoint, IDC_BTNENHCTRL_SPEC_MAX_VAL_9, DISPID_CLICK, CDialogSpecPoint::SpecInput, VTS_NONE)
	ON_EVENT(CDialogSpecPoint, IDC_BTNENHCTRL_SPEC_MAX_VAL_10, DISPID_CLICK, CDialogSpecPoint::SpecInput, VTS_NONE)
	ON_EVENT(CDialogSpecPoint, IDC_BTNENHCTRL_SPEC_MAX_VAL_11, DISPID_CLICK, CDialogSpecPoint::SpecInput, VTS_NONE)
	ON_EVENT(CDialogSpecPoint, IDC_BTNENHCTRL_SPEC_APPLY, DISPID_CLICK, CDialogSpecPoint::ClickBtnenhctrlSpecApply, VTS_NONE)
	ON_EVENT(CDialogSpecPoint, IDC_BTNENHCTRL_SPEC_CANCEL, DISPID_CLICK, CDialogSpecPoint::ClickBtnenhctrlSpecCancel, VTS_NONE)
	ON_EVENT(CDialogSpecPoint, IDC_BTNENHCTRL_SPEC_TO_SPREAD_0, DISPID_CLICK, CDialogSpecPoint::ClickBtnenhctrlSpecToSpread0, VTS_NONE)
	ON_EVENT(CDialogSpecPoint, IDC_BTNENHCTRL_SPEC_TO_SPREAD_1, DISPID_CLICK, CDialogSpecPoint::ClickBtnenhctrlSpecToSpread1, VTS_NONE)
END_EVENTSINK_MAP()


void CDialogSpecPoint::SpecInput()
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
	if(nDialogID >= IDC_BTNENHCTRL_SPEC_MAX_VAL_6)
	{
		nInspIdx = nDialogID - IDC_BTNENHCTRL_SPEC_MAX_VAL_6;
		m_ctrMinSpec[nInspIdx].GetWindowTextW(strTemp);
		dTempVal = _wtof(strTemp);
		if(dTempVal >= _wtof(strText))
		{
			strTemp.Format(_T("Max Val = %0.3f <= Min Val = %0.3f Fail !!"), _wtof(strText), dTempVal);
			AfxMessageBox(strTemp);
			return;
		}
	}
	else
	{
		nInspIdx = nDialogID - IDC_BTNENHCTRL_SPEC_MIN_VAL_6;
		m_ctrMaxSpec[nInspIdx].GetWindowTextW(strTemp);
		dTempVal = _wtof(strTemp);
		if(dTempVal <= _wtof(strText))
		{
			strTemp.Format(_T("Max Val = %0.3f <= Min Val = %0.3f Fail !!"), dTempVal, _wtof(strText));
			AfxMessageBox(strTemp);
			return;
		}
	}

	if(_ttof(strText) > 50. || _ttof(strText) < -50.) 
	{
		strTemp.Format(_T("Min Spec Val = -50.000 <= Input Val = %0.3f <= Max Spec Val = 50.000 "), _wtof(strText));
		AfxMessageBox(strTemp);
		return;
	}

	pBtnEnh->SetWindowTextW(strText);
}

void CDialogSpecPoint::UpdatePara(BOOL isUpdate, int nSpecToSpreadIdx)
{
	CString strText;
	if(isUpdate)
	{
		UpdateData(TRUE);

		if(nSpecToSpreadIdx == 0)
		{
			for(int i = 0; i < MAX_INSP_POINT_CNT; i++)
			{
				m_ctrMinSpec[i].GetWindowTextW(strText);
				m_Spec[i].dMinSpec = _wtof(strText);
			}

			for(int i = 0; i < MAX_INSP_POINT_CNT; i++)
			{
				m_ctrMaxSpec[i].GetWindowTextW(strText);
				m_Spec[i].dMaxSpec = _wtof(strText);
			}
		}
		else
		{
			for(int i = 0; i < MAX_INSP_POINT_CNT; i++)
			{
				m_ctrMinSpec[i].GetWindowTextW(strText);
				m_Spread[i].dMinSpec = _wtof(strText);
			}

			for(int i = 0; i < MAX_INSP_POINT_CNT; i++)
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
			for(int i = 0; i < MAX_INSP_POINT_CNT; i++)
			{
				strText.Format(_T("%0.3f"), m_Spec[i].dMinSpec);
				m_ctrMinSpec[i].SetCaption(strText);
			}

			for(int i = 0; i < MAX_INSP_POINT_CNT; i++)
			{
				strText.Format(_T("%0.3f"), m_Spec[i].dMaxSpec);
				m_ctrMaxSpec[i].SetCaption(strText);
			}
		}
		else
		{
			for(int i = 0; i < MAX_INSP_POINT_CNT; i++)
			{
				strText.Format(_T("%0.3f"), m_Spread[i].dMinSpec);
				m_ctrMinSpec[i].SetCaption(strText);
			}

			for(int i = 0; i < MAX_INSP_POINT_CNT; i++)
			{
				strText.Format(_T("%0.3f"), m_Spread[i].dMaxSpec);
				m_ctrMaxSpec[i].SetCaption(strText);
			}
		}

		UpdateData(FALSE);
	}
}

BOOL CDialogSpecPoint::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CDialogSpecPoint::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	
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

	}
}


BOOL CDialogSpecPoint::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if( pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE )
	{
		return TRUE;
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}


void CDialogSpecPoint::ClickBtnenhctrlSpecApply()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_isSave = TRUE;
	UpdatePara(TRUE, m_nSpecToSpreadIdx);
	ShowWindow(SW_HIDE);
}


void CDialogSpecPoint::ClickBtnenhctrlSpecCancel()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	this->ShowWindow(SW_HIDE);
}


void CDialogSpecPoint::GetSpec(TypeSpecPara *Spec)
{
	for(int i = 0; i < MAX_INSP_POINT_CNT; i++)
	{
		Spec[i] = m_Spec[i];
	}
}

void CDialogSpecPoint::SetSpec(TypeSpecPara *Spec)
{
	for(int i = 0; i < MAX_INSP_POINT_CNT; i++)
	{
		m_Spec[i] = Spec[i];
	}
}

//20190424 ngh
void CDialogSpecPoint::GetSpread(TypeSpecPara *Spread)
{
	for(int i = 0; i < MAX_INSP_POINT_CNT; i++)
	{
		Spread[i] = m_Spread[i];
	}
}

void CDialogSpecPoint::SetSpread(TypeSpecPara *Spread)
{
	for(int i = 0; i < MAX_INSP_POINT_CNT; i++)
	{
		m_Spread[i] = Spread[i];
	}
}

void CDialogSpecPoint::ClickBtnenhctrlSpecToSpread0()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_nSpecToSpreadIdx = 0;
	UpdatePara(FALSE, m_nSpecToSpreadIdx);
}


void CDialogSpecPoint::ClickBtnenhctrlSpecToSpread1()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_nSpecToSpreadIdx = 1;
	UpdatePara(FALSE, m_nSpecToSpreadIdx);
}
