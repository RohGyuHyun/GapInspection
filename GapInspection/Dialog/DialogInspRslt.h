#pragma once

struct HSORTITEM
{
    CMyListCtrl* pSortItem;
    int iColumn;
};

// CDialogInspRslt 대화 상자입니다.

class CDialogInspRslt : public CDialogEx
{
	DECLARE_DYNAMIC(CDialogInspRslt)

public:
	CDialogInspRslt(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDialogInspRslt();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_INSP_RSLT };

private:
	TypeSpecPara m_Spec[MAX_RSLT_POINT_CNT];
	TypeInspEach m_AllRslt[MAX_INSP_POINT_CNT];
	BOOL m_bRslt[MAX_INSP_POINT_CNT];
	int m_nGapRsltIdx;
	CMyListCtrl m_ctrlInspGapVal;
	CBtnenhctrl m_btn_Rslt;
	BOOL m_bAscending;
	CString m_strBarcode;
	CTime m_cStart;

	//20190307 ngh
	TypeInspRslt m_vAllRslt;
	CString m_strDrive;

	//20190410 ngh
	int m_nLR;

	//20190411 ngh
	BOOL m_bInspPointRslt[MAX_INSP_POINT_CNT];
	int m_nChannelIdx;
	BOOL m_bOKImageOpen;
	BOOL m_bNGImageOpen;
public:
	void SetSpec(TypeSpecPara *spec);
	void SetAllRsltView();
	void SetInspRsltView(int nInspPoint, BOOL *bRslt, double *dRslt);
	void InitRslt();
	void SetBarcode(CString strBarcode){m_strBarcode.Format(_T("%s"), strBarcode);};
	void SetStartTime(CTime *cStart);
	void SetDriveName(CString strDrive){m_strDrive.Format(_T("%s"), strDrive);};
	double GetRslt(int nRsltPoint, BOOL *bReturnRslt, BOOL *bReturnPointRslt, int *nRsltDirection = NULL);
	BOOL GetRslt(double *dReturnRsltArr);
	void SetLR(int nLR){m_nLR = nLR;};
	void SetPointRslt(BOOL *rslt);
	void SetChannedIndex(int nChannel){m_nChannelIdx = nChannel;};

	void SetNGImageOpen(BOOL isOpen = FALSE){m_bNGImageOpen = isOpen;};
	void SetOKImageOpen(BOOL isOpen = FALSE){m_bOKImageOpen = isOpen;};

	double GetPointRslt(int nInspPoint);
protected:
	BOOL InitList();
	BOOL InitRsltList();
	void InitRsltArr();
	void WriteRsltValList();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	DECLARE_EVENTSINK_MAP()
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void OnNMDblclkListRlstGapVal(NMHDR *pNMHDR, LRESULT *pResult);
};
