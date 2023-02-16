#pragma once

#include "ChartViewer.h"

// CDialogAvgRsltChart 대화 상자입니다.

class CDialogAvgRsltChart : public CDialogEx
{
	DECLARE_DYNAMIC(CDialogAvgRsltChart)

public:
	CDialogAvgRsltChart(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDialogAvgRsltChart();

// 대화 상자 데이터입니다.
#ifdef MODEL_BLOOM
	enum { IDD = IDD_DIALOG_AVG_RSLT_CHART_VIEW_BLOOM };
#else
	enum { IDD = IDD_DIALOG_AVG_RSLT_CHART_VIEW };
#endif

private:
	vector<TypeAvgRslt> m_vChartRslt;
	CChartViewer m_Chart;
	CBtnenhctrl m_ChartLabel[MAX_AVG_RSLT_LIST_CNT];
	int m_nChartRsltCnt;
	int m_nLabelIdx;
	TypeSpecPara m_Spec[MAX_RSLT_POINT_CNT];
	TypeSpecPara m_PointSpec[MAX_INSP_POINT_CNT];
	TypeSpecPara m_Spread[MAX_RSLT_POINT_CNT];
	TypeSpecPara m_PointSpread[MAX_INSP_POINT_CNT];
protected:
	void DrawChart();

public:
	void SetChartRslt(vector<TypeAvgRslt> vChartRslt);
	void SetChartRsltCnt(int nCnt){m_nChartRsltCnt = nCnt;};
	void SetDrawChart(){DrawChart();};
	void SetSpec(TypeSpecPara *Spec, int nSpecType = 0);
	void SetSpread(TypeSpecPara *Spread, int nSpreadType = 0);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	DECLARE_EVENTSINK_MAP()
	void ClickBtnenhctrlAvgRsltViewChartCap1();
	void ClickBtnenhctrlAvgRsltViewChartCap2();
	void ClickBtnenhctrlAvgRsltViewChartCap3();
	void ClickBtnenhctrlAvgRsltViewChartCap4();
	void ClickBtnenhctrlAvgRsltViewChartCap5();
	void ClickBtnenhctrlAvgRsltViewChartCap6();
	void ClickBtnenhctrlAvgRsltViewChartCap7();
	void ClickBtnenhctrlAvgRsltViewChartCap8();
	void ClickBtnenhctrlAvgRsltViewChartCap9();
};
