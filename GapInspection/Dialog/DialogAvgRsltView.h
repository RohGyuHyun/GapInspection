#pragma once

#include "DialogAvgRsltChart.h"

#define SPREAD_TEST

// CDialogAvgRsltView ��ȭ �����Դϴ�.

class CDialogAvgRsltView : public CDialogEx
{
	DECLARE_DYNAMIC(CDialogAvgRsltView)

public:
	CDialogAvgRsltView(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDialogAvgRsltView();

// ��ȭ ���� �������Դϴ�.
#ifdef MODEL_BLOOM
	enum { IDD = IDD_DIALOG_AVG_RSLT_VIEW_BLOOM };
#else
	enum { IDD = IDD_DIALOG_AVG_RSLT_VIEW };
#endif

private:
	int m_nPortIdx;
	int m_nAvgRsltCnt;

	vector<TypeAvgRslt> m_vPortLeftRslt;
	vector<TypeAvgRslt> m_vPortRightRslt;
	vector<TypeAvgRslt> m_vPortNonRslt;

	vector<TypeSpreadAlarmData> m_vPortLeftSpread;
	vector<TypeSpreadAlarmData> m_vPortRightSpread;
	vector<TypeSpreadAlarmData> m_vPortNonSpread;

	CMyListCtrl m_ctrlAvgRsltList[MAX_AVG_RSLT_LIST_CNT];
	CBtnenhctrl m_btn_AvgRslt[MAX_AVG_RSLT_LIST_CNT];
	CBtnenhctrl m_btn_StdRslt[MAX_AVG_RSLT_LIST_CNT];
	CBtnenhctrl m_btn_RatioRslt[MAX_RSLT_POINT_CNT];

	CDialogAvgRsltChart *m_dlgChartView;

	int m_nAlarmCnt;
	TypeSpecPara m_Spec[MAX_RSLT_POINT_CNT];
	TypeSpecPara m_PointSpec[MAX_INSP_POINT_CNT];
	TypeSpecPara m_Spread[MAX_RSLT_POINT_CNT];
	TypeSpecPara m_PointSpread[MAX_INSP_POINT_CNT];
	TypeSpreadAlarm m_SpreadAlarm;
	BOOL m_bAlarmCheckEnd;

	int m_nTestVal;
protected:
	void InitList();
	void WriteList();

	void AvgRsltViewPort0();
	void AvgRsltViewPort1();
	void AvgRsltViewPort2();

	void SpreadAlarmCheck(int nPortIdx);
	void SpreadAlarmCheckRslt(int nPortIdx);
	void InitSpreadAlarmData();

	

public:
	void SetAvgRsltCnt(int nCnt){m_nAvgRsltCnt = nCnt;};
	void SetAvgRsltVector(TypeAvgRslt vAvgRslt, int nPortIdx);
	void SetSpec(TypeSpecPara *Spec, int nSpecType = 0);
	void SetSpread(TypeSpecPara *Spread, int nSpreadType = 0);
	void SetSpreadAlarmCnt(int nCnt){m_nAlarmCnt = nCnt;};
	BOOL GetSpreadAlarm(){return m_SpreadAlarm.bAlarm;};
	void SetSpreadAlarmCheck(int nPortIdx){SpreadAlarmCheck(nPortIdx);};
	TypeSpreadAlarm GetSpreadAlarmData(){return m_SpreadAlarm;};
	void SetInitSpreadAlarmData(){InitSpreadAlarmData();};
	BOOL GetSpreadAlarmCheckEnd(){return m_bAlarmCheckEnd;};
	void SetRatio(double *dRatio);

	BOOL SpreadAlarmCheck();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	DECLARE_EVENTSINK_MAP()
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	void ClickBtnenhctrlAvgRsltViewChartView();
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
};
