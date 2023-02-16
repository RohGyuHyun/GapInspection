// Dialog\DialogAvgRsltView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "GapInspection.h"
#include "DialogAvgRsltView.h"
#include "afxdialogex.h"


// CDialogAvgRsltView 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDialogAvgRsltView, CDialogEx)

CDialogAvgRsltView::CDialogAvgRsltView(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDialogAvgRsltView::IDD, pParent)
{
	m_dlgChartView = NULL;
	m_nAlarmCnt = 5;
	InitSpreadAlarmData();

	m_nTestVal = 0;
}

CDialogAvgRsltView::~CDialogAvgRsltView()
{
	if(m_dlgChartView != NULL)
		delete m_dlgChartView;

	if(m_vPortLeftRslt.size() > 0)
		m_vPortLeftRslt.clear();

	if(m_vPortRightRslt.size() > 0)
		m_vPortRightRslt.clear();

	if(m_vPortNonRslt.size() > 0)
		m_vPortNonRslt.clear();
}

void CDialogAvgRsltView::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	int i = 0;
	DDX_Control(pDX, IDC_LIST_Y, m_ctrlAvgRsltList[i++]);
	DDX_Control(pDX, IDC_LIST_X_CAM, m_ctrlAvgRsltList[i++]);
	DDX_Control(pDX, IDC_LIST_X_USB, m_ctrlAvgRsltList[i++]);
	DDX_Control(pDX, IDC_LIST__POINT_0, m_ctrlAvgRsltList[i++]);
	DDX_Control(pDX, IDC_LIST__POINT_1, m_ctrlAvgRsltList[i++]);
	DDX_Control(pDX, IDC_LIST__POINT_2, m_ctrlAvgRsltList[i++]);
	DDX_Control(pDX, IDC_LIST__POINT_3, m_ctrlAvgRsltList[i++]);
	DDX_Control(pDX, IDC_LIST__POINT_4, m_ctrlAvgRsltList[i++]);
	DDX_Control(pDX, IDC_LIST__POINT_5, m_ctrlAvgRsltList[i++]);
	i = 0;
	DDX_Control(pDX, IDC_BTNENHCTRL_AVG_RSLT_VIEW_AVG_RSLT_0, m_btn_AvgRslt[i++]);
	DDX_Control(pDX, IDC_BTNENHCTRL_AVG_RSLT_VIEW_AVG_RSLT_1, m_btn_AvgRslt[i++]);
	DDX_Control(pDX, IDC_BTNENHCTRL_AVG_RSLT_VIEW_AVG_RSLT_2, m_btn_AvgRslt[i++]);
	DDX_Control(pDX, IDC_BTNENHCTRL_AVG_RSLT_VIEW_AVG_RSLT_3, m_btn_AvgRslt[i++]);
	DDX_Control(pDX, IDC_BTNENHCTRL_AVG_RSLT_VIEW_AVG_RSLT_4, m_btn_AvgRslt[i++]);
	DDX_Control(pDX, IDC_BTNENHCTRL_AVG_RSLT_VIEW_AVG_RSLT_5, m_btn_AvgRslt[i++]);
	DDX_Control(pDX, IDC_BTNENHCTRL_AVG_RSLT_VIEW_AVG_RSLT_6, m_btn_AvgRslt[i++]);
	DDX_Control(pDX, IDC_BTNENHCTRL_AVG_RSLT_VIEW_AVG_RSLT_7, m_btn_AvgRslt[i++]);
	DDX_Control(pDX, IDC_BTNENHCTRL_AVG_RSLT_VIEW_AVG_RSLT_8, m_btn_AvgRslt[i++]);
	i = 0;
	DDX_Control(pDX, IDC_BTNENHCTRL_AVG_RSLT_VIEW_STD_RSLT_0, m_btn_StdRslt[i++]);
	DDX_Control(pDX, IDC_BTNENHCTRL_AVG_RSLT_VIEW_STD_RSLT_1, m_btn_StdRslt[i++]);
	DDX_Control(pDX, IDC_BTNENHCTRL_AVG_RSLT_VIEW_STD_RSLT_2, m_btn_StdRslt[i++]);
	DDX_Control(pDX, IDC_BTNENHCTRL_AVG_RSLT_VIEW_STD_RSLT_3, m_btn_StdRslt[i++]);
	DDX_Control(pDX, IDC_BTNENHCTRL_AVG_RSLT_VIEW_STD_RSLT_4, m_btn_StdRslt[i++]);
	DDX_Control(pDX, IDC_BTNENHCTRL_AVG_RSLT_VIEW_STD_RSLT_5, m_btn_StdRslt[i++]);
	DDX_Control(pDX, IDC_BTNENHCTRL_AVG_RSLT_VIEW_STD_RSLT_6, m_btn_StdRslt[i++]);
	DDX_Control(pDX, IDC_BTNENHCTRL_AVG_RSLT_VIEW_STD_RSLT_7, m_btn_StdRslt[i++]);
	DDX_Control(pDX, IDC_BTNENHCTRL_AVG_RSLT_VIEW_STD_RSLT_8, m_btn_StdRslt[i++]);
	i = 0;
	DDX_Control(pDX, IDC_BTNENHCTRL_AVG_RSLT_VIEW_RATIO_RSLT_0, m_btn_RatioRslt[i++]);
	DDX_Control(pDX, IDC_BTNENHCTRL_AVG_RSLT_VIEW_RATIO_RSLT_1, m_btn_RatioRslt[i++]);
	DDX_Control(pDX, IDC_BTNENHCTRL_AVG_RSLT_VIEW_RATIO_RSLT_2, m_btn_RatioRslt[i++]);
}


BEGIN_MESSAGE_MAP(CDialogAvgRsltView, CDialogEx)
	ON_WM_TIMER()
	ON_WM_SHOWWINDOW()
	ON_WM_MEASUREITEM()
END_MESSAGE_MAP()


// CDialogAvgRsltView 메시지 처리기입니다.


void CDialogAvgRsltView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CDialogEx::OnTimer(nIDEvent);
}


BOOL CDialogAvgRsltView::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	InitList();

	/*CBtnenhctrl *pBtnEnh = (CBtnenhctrl*)GetDlgItem(IDC_BTNENHCTRL_AVG_RSLT_VIEW_PORT_2);
	pBtnEnh->SetValue(TRUE);*/
	m_nPortIdx = IDC_BTNENHCTRL_AVG_RSLT_VIEW_PORT_2;
	//WriteList();

	m_dlgChartView = new CDialogAvgRsltChart();
#ifdef MODEL_BLOOM
	m_dlgChartView->Create(IDD_DIALOG_AVG_RSLT_CHART_VIEW_BLOOM, this);
#else
	m_dlgChartView->Create(IDD_DIALOG_AVG_RSLT_CHART_VIEW, this);
#endif

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


BOOL CDialogAvgRsltView::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return CDialogEx::PreTranslateMessage(pMsg);
}

void CDialogAvgRsltView::InitList()
{
	DWORD dwStyle;
	CString strText(_T(""));
	LV_COLUMN lvcolumn; 
	TCHAR *sStepSideList[2] = {_T("Index"), _T("Value")};
	int iStepSideWidth[2] = {40, 65};
	

	for(int i = 0; i < MAX_AVG_RSLT_LIST_CNT; i++)
	{
		dwStyle = m_ctrlAvgRsltList[i].GetExtendedStyle(); 
		dwStyle |= LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EDITLABELS ;// | LVS_EX_FULLROWSELECT; 
		m_ctrlAvgRsltList[i].SetExtendedStyle(dwStyle);
		for(int j = 0; j < 2; j++)
		{
			lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH; 
			lvcolumn.fmt = LVCFMT_LEFT | LVCFMT_FIXED_WIDTH; 
			lvcolumn.pszText = sStepSideList[j]; 
			lvcolumn.iSubItem = j; 
			lvcolumn.cx = iStepSideWidth[j]; 
			m_ctrlAvgRsltList[i].InsertColumn(j, &lvcolumn); 
		}
	}
}

void CDialogAvgRsltView::SetAvgRsltVector(TypeAvgRslt vAvgRslt, int nPortIdx)
{
	switch(nPortIdx)
	{
	case PORT_BARCODE_EMPTY:
		m_vPortNonRslt.push_back(vAvgRslt);
		if(m_nAvgRsltCnt < m_vPortNonRslt.size())
		{
			vector<TypeAvgRslt>::iterator it;
			it = m_vPortNonRslt.begin();
			m_vPortNonRslt.erase(it);
		}
		break;
	case PORT_BARCODE_LEFT:
		m_vPortLeftRslt.push_back(vAvgRslt);
		if(m_nAvgRsltCnt < m_vPortLeftRslt.size())
		{
			vector<TypeAvgRslt>::iterator it;
			it = m_vPortLeftRslt.begin();
			m_vPortLeftRslt.erase(it);
		}
		break;
	case PORT_BARCODE_RIGHT:
		m_vPortRightRslt.push_back(vAvgRslt);
		if(m_nAvgRsltCnt < m_vPortRightRslt.size())
		{
			vector<TypeAvgRslt>::iterator it;
			it = m_vPortRightRslt.begin();
			m_vPortRightRslt.erase(it);
		}
		break;
	}

	if(this->IsWindowVisible())
	{
		WriteList();
		if(m_dlgChartView->IsWindowVisible())
		{
			switch(m_nPortIdx)
			{
			case IDC_BTNENHCTRL_AVG_RSLT_VIEW_PORT_0:
					m_dlgChartView->SetChartRslt(m_vPortLeftRslt);				
				break;
			case IDC_BTNENHCTRL_AVG_RSLT_VIEW_PORT_1:
					m_dlgChartView->SetChartRslt(m_vPortRightRslt);
				break;
			case IDC_BTNENHCTRL_AVG_RSLT_VIEW_PORT_2:
					m_dlgChartView->SetChartRslt(m_vPortNonRslt);
				break;
			}

			m_dlgChartView->SetChartRsltCnt(m_nAvgRsltCnt);
			m_dlgChartView->SetDrawChart();
		}
	}
}
BEGIN_EVENTSINK_MAP(CDialogAvgRsltView, CDialogEx)
	ON_EVENT(CDialogAvgRsltView, IDC_BTNENHCTRL_AVG_RSLT_VIEW_PORT_0, DISPID_CLICK, CDialogAvgRsltView::AvgRsltViewPort0, VTS_NONE)
	ON_EVENT(CDialogAvgRsltView, IDC_BTNENHCTRL_AVG_RSLT_VIEW_PORT_1, DISPID_CLICK, CDialogAvgRsltView::AvgRsltViewPort1, VTS_NONE)
	ON_EVENT(CDialogAvgRsltView, IDC_BTNENHCTRL_AVG_RSLT_VIEW_PORT_2, DISPID_CLICK, CDialogAvgRsltView::AvgRsltViewPort2, VTS_NONE)
	ON_EVENT(CDialogAvgRsltView, IDC_BTNENHCTRL_AVG_RSLT_VIEW_CHART_VIEW, DISPID_CLICK, CDialogAvgRsltView::ClickBtnenhctrlAvgRsltViewChartView, VTS_NONE)
END_EVENTSINK_MAP()


void CDialogAvgRsltView::AvgRsltViewPort0()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_nPortIdx = IDC_BTNENHCTRL_AVG_RSLT_VIEW_PORT_0;
	for(int i = 0; i < MAX_AVG_RSLT_LIST_CNT; i++)
	{
		m_ctrlAvgRsltList[i].DeleteAllItems();
	}
	WriteList();

	if(m_dlgChartView->IsWindowVisible())
	{
		switch(m_nPortIdx)
		{
		case IDC_BTNENHCTRL_AVG_RSLT_VIEW_PORT_0:
				m_dlgChartView->SetChartRslt(m_vPortLeftRslt);				
			break;
		case IDC_BTNENHCTRL_AVG_RSLT_VIEW_PORT_1:
				m_dlgChartView->SetChartRslt(m_vPortRightRslt);
			break;
		case IDC_BTNENHCTRL_AVG_RSLT_VIEW_PORT_2:
				m_dlgChartView->SetChartRslt(m_vPortNonRslt);
			break;
		}

		m_dlgChartView->SetChartRsltCnt(m_nAvgRsltCnt);
		m_dlgChartView->SetDrawChart();
	}
}

void CDialogAvgRsltView::AvgRsltViewPort1()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_nPortIdx = IDC_BTNENHCTRL_AVG_RSLT_VIEW_PORT_1;
	for(int i = 0; i < MAX_AVG_RSLT_LIST_CNT; i++)
	{
		m_ctrlAvgRsltList[i].DeleteAllItems();
	}
	WriteList();

	if(m_dlgChartView->IsWindowVisible())
	{
		switch(m_nPortIdx)
		{
		case IDC_BTNENHCTRL_AVG_RSLT_VIEW_PORT_0:
				m_dlgChartView->SetChartRslt(m_vPortLeftRslt);				
			break;
		case IDC_BTNENHCTRL_AVG_RSLT_VIEW_PORT_1:
				m_dlgChartView->SetChartRslt(m_vPortRightRslt);
			break;
		case IDC_BTNENHCTRL_AVG_RSLT_VIEW_PORT_2:
				m_dlgChartView->SetChartRslt(m_vPortNonRslt);
			break;
		}

		m_dlgChartView->SetChartRsltCnt(m_nAvgRsltCnt);
		m_dlgChartView->SetDrawChart();
	}
}

void CDialogAvgRsltView::AvgRsltViewPort2()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_nPortIdx = IDC_BTNENHCTRL_AVG_RSLT_VIEW_PORT_2;
	for(int i = 0; i < MAX_AVG_RSLT_LIST_CNT; i++)
	{
		m_ctrlAvgRsltList[i].DeleteAllItems();
	}
	WriteList();

	if(m_dlgChartView->IsWindowVisible())
	{
		switch(m_nPortIdx)
		{
		case IDC_BTNENHCTRL_AVG_RSLT_VIEW_PORT_0:
				m_dlgChartView->SetChartRslt(m_vPortLeftRslt);				
			break;
		case IDC_BTNENHCTRL_AVG_RSLT_VIEW_PORT_1:
				m_dlgChartView->SetChartRslt(m_vPortRightRslt);
			break;
		case IDC_BTNENHCTRL_AVG_RSLT_VIEW_PORT_2:
				m_dlgChartView->SetChartRslt(m_vPortNonRslt);
			break;
		}

		m_dlgChartView->SetChartRsltCnt(m_nAvgRsltCnt);
		m_dlgChartView->SetDrawChart();
	}
}

void CDialogAvgRsltView::WriteList()
{
	vector<TypeAvgRslt> vAvgRslt;
	CString strText, strDirection;
	double dSumRslt[MAX_AVG_RSLT_LIST_CNT] = {0.,}, dAvg = 0., dStd = 0.;
	double *dEachRslt;
	int k = 0;

	switch(m_nPortIdx)
	{
	case IDC_BTNENHCTRL_AVG_RSLT_VIEW_PORT_0:
		vAvgRslt = m_vPortLeftRslt;
		break;
	case IDC_BTNENHCTRL_AVG_RSLT_VIEW_PORT_1:
		vAvgRslt = m_vPortRightRslt;
		break;
	case IDC_BTNENHCTRL_AVG_RSLT_VIEW_PORT_2:
		vAvgRslt = m_vPortNonRslt;
		break;
	}

	if(vAvgRslt.size() == 0)
	{
		for(int i = 0; i < MAX_AVG_RSLT_LIST_CNT; i++)
		{
			if(i < 3)
			{
				strText.Format(_T("C 0.000"));
				m_btn_AvgRslt[i].SetWindowTextW(strText);
			
				strText.Format(_T("C 0.000"));
				m_btn_StdRslt[i].SetWindowTextW(strText);
			}
			else
			{
				strText.Format(_T("0.000"));
				m_btn_AvgRslt[i].SetWindowTextW(strText);
			
				strText.Format(_T("0.000"));
				m_btn_StdRslt[i].SetWindowTextW(strText);
			}
		}

		return;
	}
	
	for(int i = 0; i < MAX_AVG_RSLT_LIST_CNT; i++)
	{
		k = 0;
		dEachRslt = new double[vAvgRslt.size()];
		memset(dEachRslt, 0., sizeof(double) * vAvgRslt.size());
		m_ctrlAvgRsltList[i].DeleteAllItems();
		for(int j = 0; j < vAvgRslt.size(); j++)
		{
			strText.Format(_T("%d"), j + 1);
			m_ctrlAvgRsltList[i].InsertItem(0, strText);

			if(i < 3)
			{
#ifdef MODEL_BLOOM
				switch(i)
				{
				case 0:
					if(vAvgRslt[j].dGapRslt[i] > 0.)
						strDirection.Format(_T("R"));
					else if(vAvgRslt[j].dGapRslt[i] == 0.)
						strDirection.Format(_T("C"));
					else
						strDirection.Format(_T("L"));
					break;
				case 1:
				case 2:
					if(vAvgRslt[j].dGapRslt[i] > 0.)
						strDirection.Format(_T("T"));
					else if(vAvgRslt[j].dGapRslt[i] == 0.)
						strDirection.Format(_T("C"));
					else
						strDirection.Format(_T("B"));
					break;
				}
#else
				switch(i)
				{
				case 0:
					if(vAvgRslt[j].dGapRslt[i] > 0.)
						strDirection.Format(_T("T"));
					else if(vAvgRslt[j].dGapRslt[i] == 0.)
						strDirection.Format(_T("C"));
					else
						strDirection.Format(_T("B"));
					break;
				case 1:
				case 2:
					if(vAvgRslt[j].dGapRslt[i] > 0.)
						strDirection.Format(_T("R"));
					else if(vAvgRslt[j].dGapRslt[i] == 0.)
						strDirection.Format(_T("C"));
					else
						strDirection.Format(_T("L"));
					break;
				}
#endif
				strText.Format(_T("%s %0.3f"), strDirection, abs(vAvgRslt[j].dGapRslt[i]));
				m_ctrlAvgRsltList[i].SetItemText(0, 1, strText);
				dSumRslt[i] += vAvgRslt[j].dGapRslt[i];
				dEachRslt[k++] = vAvgRslt[j].dGapRslt[i];
			}
			else
			{
				strText.Format(_T("%0.3f"), vAvgRslt[j].dGapPointRslt[i - 3] / 2);
				m_ctrlAvgRsltList[i].SetItemText(0, 1, strText);
				dSumRslt[i] += vAvgRslt[j].dGapPointRslt[i - 3] / 2.;
				dEachRslt[k++] = vAvgRslt[j].dGapPointRslt[i - 3] / 2.;
			}
		}

		dAvg = dSumRslt[i] / (double)vAvgRslt.size();
		if(i < 3)
		{
#ifdef MODEL_BLOOM
			switch(i)
			{
			case 0:
				if(dAvg > 0.)
					strDirection.Format(_T("R"));
				else if(dAvg == 0.)
					strDirection.Format(_T("C"));
				else
					strDirection.Format(_T("L"));
				break;
			case 1:
			case 2:
				if(dAvg > 0.)
					strDirection.Format(_T("T"));
				else if(dAvg == 0.)
					strDirection.Format(_T("C"));
				else
					strDirection.Format(_T("B"));
				break;
			}
#else
			switch(i)
			{
			case 0:
				if(dAvg > 0.)
					strDirection.Format(_T("T"));
				else if(dAvg == 0.)
					strDirection.Format(_T("C"));
				else
					strDirection.Format(_T("B"));
				break;
			case 1:
			case 2:
				if(dAvg > 0.)
					strDirection.Format(_T("R"));
				else if(dAvg == 0.)
					strDirection.Format(_T("C"));
				else
					strDirection.Format(_T("L"));
				break;
			}
#endif
			strText.Format(_T("%s %0.3f"), strDirection, abs(dAvg));
			m_btn_AvgRslt[i].SetWindowTextW(strText);

			dStd = GetSTD(dEachRslt, vAvgRslt.size());
#ifdef MODEL_BLOOM
			switch(i)
			{
			case 0:
				if(dStd > 0.)
					strDirection.Format(_T("R"));
				else if(dStd == 0.)
					strDirection.Format(_T("C"));
				else
					strDirection.Format(_T("L"));
				break;
			case 1:
			case 2:
				if(dStd > 0.)
					strDirection.Format(_T("T"));
				else if(dStd == 0.)
					strDirection.Format(_T("C"));
				else
					strDirection.Format(_T("B"));
				break;
			}
#else
			switch(i)
			{
			case 0:
				if(dStd > 0.)
					strDirection.Format(_T("T"));
				else if(dStd == 0.)
					strDirection.Format(_T("C"));
				else
					strDirection.Format(_T("B"));
				break;
			case 1:
			case 2:
				if(dStd > 0.)
					strDirection.Format(_T("R"));
				else if(dStd == 0.)
					strDirection.Format(_T("C"));
				else
					strDirection.Format(_T("L"));
				break;
			}
#endif
			
			strText.Format(_T("%s %0.3f"), strDirection, abs(dStd));
			m_btn_StdRslt[i].SetWindowTextW(strText);
		}
		else
		{
			strText.Format(_T("%0.3f"), dAvg);
			m_btn_AvgRslt[i].SetWindowTextW(strText);

			strText.Format(_T("%0.3f"), GetSTD(dEachRslt, vAvgRslt.size()));
			m_btn_StdRslt[i].SetWindowTextW(strText);
		}

		delete dEachRslt;
	}

	vAvgRslt.clear();
}

void CDialogAvgRsltView::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if(bShow)
	{
		WriteList();
		CRect rect1, rect2, rect3;
		this->GetWindowRect(&rect1);
		GetDlgItem(IDC_BTNENHCTRL_AVG_RSLT_VIEW_CAP_1)->GetWindowRect(&rect2);
		m_dlgChartView->GetWindowRect(&rect3);
		m_dlgChartView->MoveWindow(rect1.left, rect2.top, rect3.Width(), rect3.Height());
		CBtnenhctrl *pBtnEnh = (CBtnenhctrl*)GetDlgItem(IDC_BTNENHCTRL_AVG_RSLT_VIEW_CHART_VIEW);
		if(pBtnEnh->GetValue())
		{
			CRect rect1, rect2, rect3;
			m_dlgChartView->GetWindowRect(&rect1);
			GetDlgItem(IDC_BTNENHCTRL_AVG_RSLT_VIEW_CAP_1)->GetWindowRect(&rect2);
			GetDlgItem(IDC_BTNENHCTRL_AVG_RSLT_VIEW_CAP_10)->GetWindowRect(&rect3);

			m_dlgChartView->MoveWindow(rect3.right, rect2.top, rect1.Width() - 5, rect1.Height());
			m_dlgChartView->ShowWindow(SW_SHOW);
		}
	}
	else
	{
		m_dlgChartView->ShowWindow(SW_HIDE);
	}
}


void CDialogAvgRsltView::ClickBtnenhctrlAvgRsltViewChartView()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CBtnenhctrl *pBtnEnh = (CBtnenhctrl*)GetDlgItem(IDC_BTNENHCTRL_AVG_RSLT_VIEW_CHART_VIEW);
	if(m_dlgChartView->IsWindowVisible())
	{
		m_dlgChartView->ShowWindow(SW_HIDE);
		pBtnEnh->SetValue(FALSE);
	}
	else
	{
		pBtnEnh->SetValue(TRUE);

		switch(m_nPortIdx)
		{
		case IDC_BTNENHCTRL_AVG_RSLT_VIEW_PORT_0:
				m_dlgChartView->SetChartRslt(m_vPortLeftRslt);				
			break;
		case IDC_BTNENHCTRL_AVG_RSLT_VIEW_PORT_1:
				m_dlgChartView->SetChartRslt(m_vPortRightRslt);
			break;
		case IDC_BTNENHCTRL_AVG_RSLT_VIEW_PORT_2:
				m_dlgChartView->SetChartRslt(m_vPortNonRslt);
			break;
		}

		m_dlgChartView->SetChartRsltCnt(m_nAvgRsltCnt);
		m_dlgChartView->ShowWindow(SW_SHOW);

		CRect rect1, rect2, rect3;
		m_dlgChartView->GetWindowRect(&rect1);
		GetDlgItem(IDC_BTNENHCTRL_AVG_RSLT_VIEW_CAP_1)->GetWindowRect(&rect2);
		GetDlgItem(IDC_BTNENHCTRL_AVG_RSLT_VIEW_CAP_10)->GetWindowRect(&rect3);

		m_dlgChartView->MoveWindow(rect3.right, rect2.top, rect1.Width() - 5, rect1.Height());

	}
}

void CDialogAvgRsltView::SetSpec(TypeSpecPara *Spec, int nSpecType)
{
	if(nSpecType == INSP_SPEC_TYPE_0)
	{
		for(int i = 0; i < MAX_RSLT_POINT_CNT; i++)
		{
			m_Spec[i].dMinSpec = Spec[i].dMinSpec;
			m_Spec[i].dMaxSpec = Spec[i].dMaxSpec;
			m_Spec[i].dOffset = Spec[i].dOffset;
		}
	}
	else
	{
		for(int i = 0; i < MAX_INSP_POINT_CNT; i++)
		{
			m_PointSpec[i].dMinSpec = Spec[i].dMinSpec;
			m_PointSpec[i].dMaxSpec = Spec[i].dMaxSpec;
			m_PointSpec[i].dOffset = Spec[i].dOffset;
		}
	}

	m_dlgChartView->SetSpec(Spec, nSpecType);
}

void CDialogAvgRsltView::SetSpread(TypeSpecPara *Spread, int nSpreadType)
{
	if(nSpreadType == INSP_SPEC_TYPE_0)
	{
		for(int i = 0; i < MAX_RSLT_POINT_CNT; i++)
		{
			m_Spread[i].dMinSpec = Spread[i].dMinSpec;
			m_Spread[i].dMaxSpec = Spread[i].dMaxSpec;
			m_Spread[i].dOffset = Spread[i].dOffset;
		}
	} 
	else
	{
		for(int i = 0; i < MAX_INSP_POINT_CNT; i++)
		{
			m_PointSpread[i].dMinSpec = Spread[i].dMinSpec;
			m_PointSpread[i].dMaxSpec = Spread[i].dMaxSpec;
			m_PointSpread[i].dOffset = Spread[i].dOffset;
		}
	}

	m_dlgChartView->SetSpread(Spread, nSpreadType);
}

BOOL CDialogAvgRsltView::SpreadAlarmCheck()
{
	BOOL rslt = FALSE;

	if(m_nAlarmCnt == 0)
	{
		m_bAlarmCheckEnd = TRUE;
		m_SpreadAlarm.bAlarm = FALSE;
		return rslt;
	}

	vector<TypeAvgRslt> vRslt;

	vRslt = m_vPortNonRslt;

	if(vRslt.size() == 0 || vRslt.size() < m_nAlarmCnt)
		return rslt;

	double dSpreadMin[MAX_AVG_RSLT_LIST_CNT] = {0.,}, dSpreadMax[MAX_AVG_RSLT_LIST_CNT] = {0.,}, dRslt = 0.;
	int nDirectionIdx;
	vector<TypeSpreadAlarmData> vSpreadRslt;
	TypeSpreadAlarmData SpreadAlarmData;

	for(int i = 0; i < MAX_RSLT_POINT_CNT; i++)
	{
		dSpreadMin[i] = m_Spec[i].dMinSpec + m_Spread[i].dMinSpec;
		dSpreadMax[i] = m_Spec[i].dMaxSpec - m_Spread[i].dMaxSpec;
	}

	for(int i = 0; i < m_nAlarmCnt; i++)
	{
		for(int j = 0; j < MAX_RSLT_POINT_CNT; j++)
		{
			dRslt = vRslt[vRslt.size() - 1 - i].dGapRslt[j];

			if(dSpreadMax[j] >= dRslt && dRslt >= dSpreadMin[j])
			{
				SpreadAlarmData.nDirection[j] = -1;
				SpreadAlarmData.bSpreadAlarm[j] = FALSE;
			}
			else
			{
				if(dRslt> 0.)
					nDirectionIdx = RIGHT_OR_TOP_DIRECTION;
				else if(dRslt < 0.)
					nDirectionIdx = LEFT_OR_BOT_DIRECTION;
				else
					nDirectionIdx = 0;

				SpreadAlarmData.nDirection[j] = nDirectionIdx;
				SpreadAlarmData.bSpreadAlarm[j] = TRUE;
			}
		}
		vSpreadRslt.push_back(SpreadAlarmData);
	}

	int *nDirection;
	BOOL *isSpreadRslt;
	CString strText1, strText2, strText;
	strText1 = _T("");
	strText2 = _T("");
	strText = _T("");

	nDirection = new int[MAX_RSLT_POINT_CNT];
	isSpreadRslt = new BOOL[MAX_RSLT_POINT_CNT];
	memset(nDirection, 0, sizeof(int) * MAX_RSLT_POINT_CNT);
	memset(isSpreadRslt, FALSE, sizeof(BOOL) * MAX_RSLT_POINT_CNT);
	memset(m_SpreadAlarm.cAlarmString, NULL, 256);
	
	for(int i = 0; i < MAX_RSLT_POINT_CNT; i++)
	{
		BOOL isRslt = vSpreadRslt[0].bSpreadAlarm[i];
		int nDirection1 = vSpreadRslt[0].nDirection[i];

		if(!isRslt)
			continue;

		for(int j = 1; j < vSpreadRslt.size(); j++)
		{
			if(isRslt != vSpreadRslt[j].bSpreadAlarm[i] || nDirection1 != vSpreadRslt[j].nDirection[i])
			{
				isRslt = FALSE;
			}
		}

		if(isRslt)
		{
			nDirection[i] = nDirection1;
			isSpreadRslt[i] = isRslt;
			if(i == 0)
			{
				strText1.Format(_T("Y (1 - 4) Spread Alarm\n"));
			}
			else if( i == 1)
			{
				strText1.Format(_T("X Right (2 - 3) Spread Alarm\n"));
			}
			else
			{
				strText1.Format(_T("X Left (5 - 6) Spread Alarm\n"));
			}
			/*switch(nDirection1)
			{
			case RIGHT_OR_TOP_DIRECTION:
				if(i == 0)
				{
					strText1.Format(_T("Y (1 - 4) Right Direction\n"));
				}
				else if( i == 1)
				{
					strText1.Format(_T("X Right (2 - 3) CAM Direction\n"));
				}
				else
				{
					strText1.Format(_T("X Left (5 - 6) CAM Direction\n"));
				}
				break;
			case LEFT_OR_BOT_DIRECTION:
				if(i == 0)
				{
					strText1.Format(_T("Y (1 - 4) Left Direction\n"));
				}
				else if( i == 1)
				{
					strText1.Format(_T("X Right (2 - 3) USB Direction\n"));
				}
				else
				{
					strText1.Format(_T("X Left (5 - 6) USB Direction\n"));
				}
				break;
			}*/

			strText2.Format(_T("%S%s"), m_SpreadAlarm.cAlarmString, strText1);
			sprintf_s(m_SpreadAlarm.cAlarmString, strText2.GetLength() + 1, "%S", strText2);
			rslt = TRUE;
		}
	}

	delete nDirection;
	delete isSpreadRslt;

	return rslt;
}


void CDialogAvgRsltView::SpreadAlarmCheck(int nPortIdx)
{
	if(m_nAlarmCnt == 0)
	{
		m_bAlarmCheckEnd = TRUE;
		m_SpreadAlarm.bAlarm = FALSE;
		return;
	}

	vector<TypeAvgRslt> vRslt;

	switch(nPortIdx)
	{
	case PORT_BARCODE_EMPTY:
		vRslt = m_vPortNonRslt;
		break;
	case PORT_BARCODE_LEFT:
		vRslt = m_vPortLeftRslt;
		break;
	case PORT_BARCODE_RIGHT:
		vRslt = m_vPortRightRslt;		
		break;
	}

	if(vRslt.size() == 0)
		return;

	double dSpreadMin[MAX_AVG_RSLT_LIST_CNT] = {0.,}, dSpreadMax[MAX_AVG_RSLT_LIST_CNT] = {0.,}, dRslt = 0.;
	int nDirectionIdx, nSearchEndIdx;
	vector<TypeSpreadAlarmData> vSpreadRslt;
	TypeSpreadAlarmData SpreadAlarmData;

	for(int i = 0; i < 3; i++)
	{
		if(i < 3)
		{
			dSpreadMin[i] = m_Spec[i].dMinSpec + m_Spread[i].dMinSpec;
			dSpreadMax[i] = m_Spec[i].dMaxSpec - m_Spread[i].dMaxSpec;
		}
		else
		{
			dSpreadMin[i] = m_PointSpec[i - 3].dMinSpec + m_PointSpread[i - 3].dMinSpec;
			dSpreadMax[i] = m_PointSpec[i - 3].dMaxSpec - m_PointSpread[i - 3].dMaxSpec;
		}
	}

	nSearchEndIdx = vRslt.size() - m_nAlarmCnt - 1;

	if(nSearchEndIdx < 0 || (vRslt.size() - m_nAlarmCnt) == 0)
	{
		nSearchEndIdx = -1;
	}

	for(int nRsltIdx = vRslt.size() - 1;  nRsltIdx > nSearchEndIdx; nRsltIdx--)
	{
		for(int i = 0; i < 3; i++)
		{
			if(i < 3)
			{
				dRslt = vRslt[nRsltIdx].dGapRslt[i];
			}
			else
			{
				dRslt = vRslt[nRsltIdx].dGapPointRslt[i - 3];
			}


			if(dSpreadMax[i] >= dRslt && dRslt >= dSpreadMin[i])
			{
				SpreadAlarmData.nDirection[i] = -1;
				SpreadAlarmData.bSpreadAlarm[i] = FALSE;
			}
			else
			{
				if(dRslt> 0.)
					nDirectionIdx = RIGHT_OR_TOP_DIRECTION;
				else
					nDirectionIdx = LEFT_OR_BOT_DIRECTION;

				SpreadAlarmData.nDirection[i] = nDirectionIdx;
				SpreadAlarmData.bSpreadAlarm[i] = TRUE;
			}
		}

		vSpreadRslt.push_back(SpreadAlarmData);
	}

	switch(nPortIdx)
	{
	case PORT_BARCODE_EMPTY:
		m_vPortNonSpread.clear();
		m_vPortNonSpread = vSpreadRslt;
		break;
	case PORT_BARCODE_LEFT:
		m_vPortLeftSpread.clear();
		m_vPortLeftSpread = vSpreadRslt;
		break;
	case PORT_BARCODE_RIGHT:
		m_vPortRightSpread.clear();
		m_vPortRightSpread = vSpreadRslt;
		break;
	}

	vSpreadRslt.clear();

	SpreadAlarmCheckRslt(nPortIdx);

	return;
}

void CDialogAvgRsltView::SpreadAlarmCheckRslt(int nPortIdx)
{
	vector<TypeSpreadAlarmData> vSpreadRslt;
	
	switch(nPortIdx)
	{
	case PORT_BARCODE_EMPTY:
		vSpreadRslt = m_vPortNonSpread;
		break;
	case PORT_BARCODE_LEFT:
		vSpreadRslt = m_vPortLeftSpread;
		break;
	case PORT_BARCODE_RIGHT:
		vSpreadRslt = m_vPortRightSpread;
		break;
	}

	if(vSpreadRslt.size() == 0)
	{
		vSpreadRslt.clear();
		m_bAlarmCheckEnd = TRUE;
		return;
	}

	if(m_nAlarmCnt == 0)
	{
		vSpreadRslt.clear();
		m_bAlarmCheckEnd = TRUE;
		m_SpreadAlarm.bAlarm = FALSE;
		return;
	}

	CString strPortText, strPointText, strText;
	BOOL bSpreadAlarmRslt = FALSE, bSpreadRslt = FALSE;
	BOOL *bEachRslt, bSearch = FALSE;
	int nPointIdx = -1;
	m_SpreadAlarm.bAlarm = FALSE;
	int nSpreadDirection[3];
	nSpreadDirection[0] = vSpreadRslt[0].nDirection[0];
	nSpreadDirection[1] = vSpreadRslt[0].nDirection[1];
	nSpreadDirection[2] = vSpreadRslt[0].nDirection[2];

	if(vSpreadRslt.size() >= m_nAlarmCnt)
	{
		bEachRslt = new BOOL[m_nAlarmCnt];
		memset(bEachRslt, TRUE, sizeof(BOOL) * m_nAlarmCnt);

		for(int j = 0; j < m_nAlarmCnt; j++)
		{
			for(int i = 0; i < 3; i++)
			{
				bEachRslt[j] = FALSE;
				if(vSpreadRslt[j].bSpreadAlarm[i])
				{
					if(vSpreadRslt[j].nDirection[i] == nSpreadDirection[i])
					{
						bEachRslt[j] = TRUE;
						nPointIdx = i;
						break;
					}
				}
			}
		}

		int nSpreadCnt = 0;
		bSpreadAlarmRslt = FALSE;
		for(int i = 0; i < m_nAlarmCnt; i++)
		{
			if(bEachRslt[i])
			{
				nSpreadCnt++;
			}
		}

		if(nSpreadCnt == m_nAlarmCnt)
		{
			bSpreadAlarmRslt = TRUE;
		}

		if(bSpreadAlarmRslt)
		{
			m_SpreadAlarm.bAlarm = TRUE;
			switch(nPortIdx)
			{
			case PORT_BARCODE_EMPTY:
				strPortText.Format(_T("Empty Port"));
				break;
			case PORT_BARCODE_LEFT:
				strPortText.Format(_T("Left Port"));
				break;
			case PORT_BARCODE_RIGHT:
				strPortText.Format(_T("Right Port"));
				break;
			}

#ifdef MODEL_BLOOM
			switch(nPointIdx)
			{
			case 0:
				strPointText.Format(_T("Y(1-4)"));
				break;
			case 1:
				strPointText.Format(_T("X RIGHT(2-3)"));
				break;
			case 2:
				strPointText.Format(_T("X LEFT(5-6)"));
				break;
			case 3:
				strPointText.Format(_T("Y RIGHT"));
				break;
			case 4:
				strPointText.Format(_T("X CAM RIGHT"));
				break;
			case 5:
				strPointText.Format(_T("X USB RIGHT"));
				break;
			case 6:
				strPointText.Format(_T("Y LEFT"));
				break;
			case 7:
				strPointText.Format(_T("X CAM LEFT"));
				break;
			case 8:
				strPointText.Format(_T("X USB LEFT"));
				break;
			}
#else
			switch(nPointIdx)
			{
			case 0:
				strPointText.Format(_T("Y(1-4)"));
				break;
			case 1:
				strPointText.Format(_T("X CAM(2-3)"));
				break;
			case 2:
				strPointText.Format(_T("X USB(5-6)"));
				break;
			case 3:
				strPointText.Format(_T("Y CAM"));
				break;
			case 4:
				strPointText.Format(_T("X CAM RIGHT"));
				break;
			case 5:
				strPointText.Format(_T("X CAM LEFT"));
				break;
			case 6:
				strPointText.Format(_T("Y USB"));
				break;
			case 7:
				strPointText.Format(_T("X USB RIGHT"));
				break;
			case 8:
				strPointText.Format(_T("X USB LEFT"));
				break;
			}
#endif

			strText.Format(_T("%s Spread Alarm"), strPointText);
			sprintf_s(m_SpreadAlarm.cAlarmString, strText.GetLength() + 1, "%S", strText);
		}

		delete bEachRslt;
	}

	vSpreadRslt.clear();

	m_bAlarmCheckEnd = TRUE;
}

void CDialogAvgRsltView::InitSpreadAlarmData()
{
	m_bAlarmCheckEnd = FALSE;
	m_SpreadAlarm.bAlarm = FALSE;
	memset(m_SpreadAlarm.cAlarmString, NULL, sizeof(char) * 256);
}

void CDialogAvgRsltView::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	/*if(GetDlgItem(IDC_LIST_Y) || GetDlgItem(IDC_LIST_X_CAM) || GetDlgItem(IDC_LIST_X_USB) || GetDlgItem(IDC_LIST__POINT_0))
	{
		lpMeasureItemStruct->itemHeight += 6;
	}*/

	CDialogEx::OnMeasureItem(nIDCtl, lpMeasureItemStruct);
}


void CDialogAvgRsltView::SetRatio(double *dRatio)
{
	for(int i = 0; i < MAX_RSLT_POINT_CNT; i++)
	{
		CString strText;
		strText.Format(_T("%.2f%%"), dRatio[i]);
		m_btn_RatioRslt[i].SetWindowTextW(strText);
	}
}