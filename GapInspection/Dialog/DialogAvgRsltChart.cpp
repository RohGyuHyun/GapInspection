// Dialog\DialogAvgRsltChart.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "GapInspection.h"
#include "DialogAvgRsltChart.h"
#include "afxdialogex.h"


// CDialogAvgRsltChart 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDialogAvgRsltChart, CDialogEx)

CDialogAvgRsltChart::CDialogAvgRsltChart(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDialogAvgRsltChart::IDD, pParent)
{
	m_nLabelIdx = 0;
	for(int i = 0; i < MAX_RSLT_POINT_CNT; i++)
	{
		m_Spread[i].dMinSpec = 0.05;
		m_Spread[i].dMaxSpec = 0.05;
		m_Spread[i].dOffset = 0;
	}
	for(int i = 0; i < MAX_INSP_POINT_CNT; i++)
	{
		m_PointSpread[i].dMinSpec = 0.05;
		m_PointSpread[i].dMaxSpec = 0.05;
		m_PointSpread[i].dOffset = 0;
	}
}

CDialogAvgRsltChart::~CDialogAvgRsltChart()
{
}

void CDialogAvgRsltChart::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	int i = 0;
	DDX_Control(pDX, IDC_STATIC_CHART_0, m_Chart);
	i = 0;
	DDX_Control(pDX, IDC_BTNENHCTRL_AVG_RSLT_VIEW_CHART_CAP_1, m_ChartLabel[i++]);
	DDX_Control(pDX, IDC_BTNENHCTRL_AVG_RSLT_VIEW_CHART_CAP_2, m_ChartLabel[i++]);
	DDX_Control(pDX, IDC_BTNENHCTRL_AVG_RSLT_VIEW_CHART_CAP_3, m_ChartLabel[i++]);
	DDX_Control(pDX, IDC_BTNENHCTRL_AVG_RSLT_VIEW_CHART_CAP_4, m_ChartLabel[i++]);
	DDX_Control(pDX, IDC_BTNENHCTRL_AVG_RSLT_VIEW_CHART_CAP_5, m_ChartLabel[i++]);
	DDX_Control(pDX, IDC_BTNENHCTRL_AVG_RSLT_VIEW_CHART_CAP_6, m_ChartLabel[i++]);
	DDX_Control(pDX, IDC_BTNENHCTRL_AVG_RSLT_VIEW_CHART_CAP_7, m_ChartLabel[i++]);
	DDX_Control(pDX, IDC_BTNENHCTRL_AVG_RSLT_VIEW_CHART_CAP_8, m_ChartLabel[i++]);
	DDX_Control(pDX, IDC_BTNENHCTRL_AVG_RSLT_VIEW_CHART_CAP_9, m_ChartLabel[i++]);
}


BEGIN_MESSAGE_MAP(CDialogAvgRsltChart, CDialogEx)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// CDialogAvgRsltChart 메시지 처리기입니다.
void CDialogAvgRsltChart::SetChartRslt(vector<TypeAvgRslt> vChartRslt)
{
	m_vChartRslt.clear();

	m_vChartRslt = vChartRslt;
}

void CDialogAvgRsltChart::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if(bShow)
	{
		CBtnenhctrl *pBtnEnh = (CBtnenhctrl*)GetDlgItem(IDC_BTNENHCTRL_AVG_RSLT_VIEW_CHART_CAP_1);
		pBtnEnh->SetValue(TRUE);
		DrawChart();
	}
}


BOOL CDialogAvgRsltChart::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


BOOL CDialogAvgRsltChart::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return CDialogEx::PreTranslateMessage(pMsg);
}

void CDialogAvgRsltChart::SetSpec(TypeSpecPara *Spec, int nSpecType)
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
}

void CDialogAvgRsltChart::SetSpread(TypeSpecPara *Spread, int nSpreadType)
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
}

void CDialogAvgRsltChart::DrawChart()
{
	CBtnenhctrl *pBtnEnh = (CBtnenhctrl*)GetDlgItem(IDC_BTNENHCTRL_AVG_RSLT_VIEW_CHART_CAP_12);
	CRect rect1;
	CString strTemp;
	GetDlgItem(IDC_STATIC_CHART_SIZE)->GetWindowRect(&rect1);
	char **strLabel, strMarkText[10];
	double *dData, dMinData, dMaxData, dSpreadMin, dSpreadMax;
	int nLabelSize = 0;
	int nSelectLabelIdx = m_nLabelIdx;
	if(m_vChartRslt.size() == 0)
	{
		pBtnEnh->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_CHART_0)->ShowWindow(SW_HIDE);
		return;
	}
	pBtnEnh->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_CHART_0)->ShowWindow(SW_SHOW);

	strLabel = new char*[m_nChartRsltCnt];
	dData = new double[m_nChartRsltCnt];

	for(int i = 0; i < m_nChartRsltCnt; i++)
	{
		strLabel[i] = new char[3];
		strTemp.Format(_T("%02d"), i + 1);
		sprintf_s(strLabel[i], strTemp.GetLength() + 1, "%S", strTemp);
		dData[i] = 0.;
		nLabelSize += sizeof(strLabel[i]);
	}

	if(nSelectLabelIdx < 3)
	{
		dMinData = m_Spec[nSelectLabelIdx].dMinSpec;
		dMaxData = m_Spec[nSelectLabelIdx].dMaxSpec;
		dSpreadMin = m_Spread[nSelectLabelIdx].dMinSpec;
		dSpreadMax = m_Spread[nSelectLabelIdx].dMaxSpec;
	}
	else
	{
		dMinData = m_PointSpec[nSelectLabelIdx - 3].dMinSpec;
		dMaxData = m_PointSpec[nSelectLabelIdx - 3].dMaxSpec;
		dSpreadMin = m_PointSpread[nSelectLabelIdx - 3].dMinSpec;
		dSpreadMax = m_PointSpread[nSelectLabelIdx - 3].dMaxSpec;
	}
	
	for(int j = 0; j < m_vChartRslt.size(); j++)
	{
		if(nSelectLabelIdx < 3)
		{
			dData[j] = m_vChartRslt[j].dGapRslt[nSelectLabelIdx];
		}
		else
		{
			dData[j] = m_vChartRslt[j].dGapPointRslt[nSelectLabelIdx - 3] / 2.;
		}

		if(dData[j] > dMaxData + (dSpreadMax * 2))
		{
			dData[j] = dMaxData + (dSpreadMax * 2);
		}

		if(dData[j] < dMinData - (dSpreadMin * 2))
		{
			dData[j] = dMinData - (dSpreadMin * 2);
		}
	}

	XYChart *c = new XYChart(rect1.Width(), rect1.Height(), 0xffdddd, 0x000000, 1);
	c->setRoundedFrame();

	// Set the plotarea at (55, 58) and of size 520 x 195 pixels, with white (ffffff) background.
	// Set horizontal and vertical grid lines to grey (cccccc).
	c->setPlotArea(70, 60, rect1.Width() - 90, rect1.Height() - 120, 0xffffff, -1, -1, 0xcccccc, 0xcccccc);

	// Add a legend box at (55, 32) (top of the chart) with horizontal layout. Use 9pt Arial Bold
	// font. Set the background and border color to Transparent.
	c->addLegend(70, 30, false, "arialbd.ttf", 10)->setBackground(Chart::Transparent);

	c->xAxis()->setLabelStyle("arialbd.ttf", 10);
	c->yAxis()->setLabelStyle("arialbd.ttf", 10);
	// Add a title to the y axis
	c->yAxis()->setTitle("Value");

	// Set the labels on the x axis
	//c->xAxis()->setLabels(StringArray(labels, (int)(sizeof(labels) / sizeof(labels[0]))));
	c->xAxis()->setLabels(StringArray(strLabel, (int)((sizeof(strLabel) * m_vChartRslt.size()) / sizeof(strLabel[0]))));
	
	// Add a title to the x axis using CMDL
	c->xAxis()->setTitle("Index");

	// Set the axes width to 2 pixels
	c->xAxis()->setWidth(2);
	c->yAxis()->setWidth(2);

	if(nSelectLabelIdx < 3)
	{
		c->setAxisAtOrigin(Chart::XAxisAtOrigin, Chart::YAxisSymmetric);
		
	}
	c->yAxis()->setLinearScale(dMinData - (dSpreadMin * 2), dMaxData + (dSpreadMax * 2));

	LineLayer *lineLayer = c->addLineLayer();
	lineLayer->addDataSet(DoubleArray(dData, (int)((sizeof(dData) * m_vChartRslt.size()) / sizeof(dData[0]))), 0x0000c0, "Value")->setDataSymbol(Chart::CircleSymbol, 9, 0xffff00);
	lineLayer->setLineWidth(2);

	//lineLayer->setXData(dMinData - (dSpreadMin * 2), dMaxData + (dSpreadMax * 2));
	
	strTemp.Format(_T("%0.3f"), dMinData);
	sprintf_s(strMarkText, strTemp.GetLength() + 1, "%S", strTemp);
	c->yAxis()->addMark(dMinData, 0x00ff00, strMarkText, "arialbd.ttf", 10)->setLineWidth(2);
	c->yAxis()->addMark(dMinData, 0x00ff00, strMarkText, "arialbd.ttf", 10)->setFontColor(0x000000);

	strTemp.Format(_T("%0.3f"), dMaxData);
	sprintf_s(strMarkText, strTemp.GetLength() + 1, "%S", strTemp);
	c->yAxis()->addMark(dMaxData, 0x00ff00, strMarkText, "arialbd.ttf", 10)->setLineWidth(2);
	c->yAxis()->addMark(dMaxData, 0x00ff00, strMarkText, "arialbd.ttf", 10)->setFontColor(0x000000);

	

	c->yAxis()->addZone(-5, 5, 0xff0000);
	c->yAxis()->addZone(dMinData, dMaxData, 0x00ff00);
	c->yAxis()->addZone(dMinData, dMinData + dSpreadMin, 0xffff00);
	c->yAxis()->addZone(dMaxData, dMaxData - dSpreadMax, 0xffff00);
	
	m_Chart.setChart(c);
	

	delete c;

	for(int j = 0; j < m_nChartRsltCnt; j++)
	{
		delete strLabel[j];
	}

	delete strLabel;
	delete dData;
}

BEGIN_EVENTSINK_MAP(CDialogAvgRsltChart, CDialogEx)
	ON_EVENT(CDialogAvgRsltChart, IDC_BTNENHCTRL_AVG_RSLT_VIEW_CHART_CAP_1, DISPID_CLICK, CDialogAvgRsltChart::ClickBtnenhctrlAvgRsltViewChartCap1, VTS_NONE)
	ON_EVENT(CDialogAvgRsltChart, IDC_BTNENHCTRL_AVG_RSLT_VIEW_CHART_CAP_2, DISPID_CLICK, CDialogAvgRsltChart::ClickBtnenhctrlAvgRsltViewChartCap2, VTS_NONE)
	ON_EVENT(CDialogAvgRsltChart, IDC_BTNENHCTRL_AVG_RSLT_VIEW_CHART_CAP_3, DISPID_CLICK, CDialogAvgRsltChart::ClickBtnenhctrlAvgRsltViewChartCap3, VTS_NONE)
	ON_EVENT(CDialogAvgRsltChart, IDC_BTNENHCTRL_AVG_RSLT_VIEW_CHART_CAP_4, DISPID_CLICK, CDialogAvgRsltChart::ClickBtnenhctrlAvgRsltViewChartCap4, VTS_NONE)
	ON_EVENT(CDialogAvgRsltChart, IDC_BTNENHCTRL_AVG_RSLT_VIEW_CHART_CAP_5, DISPID_CLICK, CDialogAvgRsltChart::ClickBtnenhctrlAvgRsltViewChartCap5, VTS_NONE)
	ON_EVENT(CDialogAvgRsltChart, IDC_BTNENHCTRL_AVG_RSLT_VIEW_CHART_CAP_6, DISPID_CLICK, CDialogAvgRsltChart::ClickBtnenhctrlAvgRsltViewChartCap6, VTS_NONE)
	ON_EVENT(CDialogAvgRsltChart, IDC_BTNENHCTRL_AVG_RSLT_VIEW_CHART_CAP_7, DISPID_CLICK, CDialogAvgRsltChart::ClickBtnenhctrlAvgRsltViewChartCap7, VTS_NONE)
	ON_EVENT(CDialogAvgRsltChart, IDC_BTNENHCTRL_AVG_RSLT_VIEW_CHART_CAP_8, DISPID_CLICK, CDialogAvgRsltChart::ClickBtnenhctrlAvgRsltViewChartCap8, VTS_NONE)
	ON_EVENT(CDialogAvgRsltChart, IDC_BTNENHCTRL_AVG_RSLT_VIEW_CHART_CAP_9, DISPID_CLICK, CDialogAvgRsltChart::ClickBtnenhctrlAvgRsltViewChartCap9, VTS_NONE)
END_EVENTSINK_MAP()


void CDialogAvgRsltChart::ClickBtnenhctrlAvgRsltViewChartCap1()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_nLabelIdx = 0;
	DrawChart();
	CString strText;
	GetDlgItem(IDC_BTNENHCTRL_AVG_RSLT_VIEW_CHART_CAP_1)->GetWindowTextW(strText);
	GetDlgItem(IDC_BTNENHCTRL_AVG_RSLT_VIEW_CHART_CAP_11)->SetWindowTextW(strText);
}

void CDialogAvgRsltChart::ClickBtnenhctrlAvgRsltViewChartCap2()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_nLabelIdx = 1;
	DrawChart();
	CString strText;
	GetDlgItem(IDC_BTNENHCTRL_AVG_RSLT_VIEW_CHART_CAP_2)->GetWindowTextW(strText);
	GetDlgItem(IDC_BTNENHCTRL_AVG_RSLT_VIEW_CHART_CAP_11)->SetWindowTextW(strText);
}

void CDialogAvgRsltChart::ClickBtnenhctrlAvgRsltViewChartCap3()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_nLabelIdx = 2;
	DrawChart();
	CString strText;
	GetDlgItem(IDC_BTNENHCTRL_AVG_RSLT_VIEW_CHART_CAP_3)->GetWindowTextW(strText);
	GetDlgItem(IDC_BTNENHCTRL_AVG_RSLT_VIEW_CHART_CAP_11)->SetWindowTextW(strText);
}

void CDialogAvgRsltChart::ClickBtnenhctrlAvgRsltViewChartCap4()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_nLabelIdx = 3;
	DrawChart();
	CString strText;
	GetDlgItem(IDC_BTNENHCTRL_AVG_RSLT_VIEW_CHART_CAP_4)->GetWindowTextW(strText);
	GetDlgItem(IDC_BTNENHCTRL_AVG_RSLT_VIEW_CHART_CAP_11)->SetWindowTextW(strText);
}

void CDialogAvgRsltChart::ClickBtnenhctrlAvgRsltViewChartCap5()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_nLabelIdx = 4;
	DrawChart();
	CString strText;
	GetDlgItem(IDC_BTNENHCTRL_AVG_RSLT_VIEW_CHART_CAP_5)->GetWindowTextW(strText);
	GetDlgItem(IDC_BTNENHCTRL_AVG_RSLT_VIEW_CHART_CAP_11)->SetWindowTextW(strText);
}

void CDialogAvgRsltChart::ClickBtnenhctrlAvgRsltViewChartCap6()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_nLabelIdx = 5;
	DrawChart();
	CString strText;
	GetDlgItem(IDC_BTNENHCTRL_AVG_RSLT_VIEW_CHART_CAP_6)->GetWindowTextW(strText);
	GetDlgItem(IDC_BTNENHCTRL_AVG_RSLT_VIEW_CHART_CAP_11)->SetWindowTextW(strText);
}

void CDialogAvgRsltChart::ClickBtnenhctrlAvgRsltViewChartCap7()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_nLabelIdx = 6;
	DrawChart();
	CString strText;
	GetDlgItem(IDC_BTNENHCTRL_AVG_RSLT_VIEW_CHART_CAP_7)->GetWindowTextW(strText);
	GetDlgItem(IDC_BTNENHCTRL_AVG_RSLT_VIEW_CHART_CAP_11)->SetWindowTextW(strText);
}

void CDialogAvgRsltChart::ClickBtnenhctrlAvgRsltViewChartCap8()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_nLabelIdx = 7;
	DrawChart();
	CString strText;
	GetDlgItem(IDC_BTNENHCTRL_AVG_RSLT_VIEW_CHART_CAP_8)->GetWindowTextW(strText);
	GetDlgItem(IDC_BTNENHCTRL_AVG_RSLT_VIEW_CHART_CAP_11)->SetWindowTextW(strText);
}

void CDialogAvgRsltChart::ClickBtnenhctrlAvgRsltViewChartCap9()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_nLabelIdx = 8;
	DrawChart();
	CString strText;
	GetDlgItem(IDC_BTNENHCTRL_AVG_RSLT_VIEW_CHART_CAP_9)->GetWindowTextW(strText);
	GetDlgItem(IDC_BTNENHCTRL_AVG_RSLT_VIEW_CHART_CAP_11)->SetWindowTextW(strText);
}