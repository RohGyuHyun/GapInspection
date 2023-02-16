#pragma once

#define INIT_RSLT_BTN					2
// CDialogInspPoint ��ȭ �����Դϴ�.

class CDialogInspPoint : public CDialogEx
{
	DECLARE_DYNAMIC(CDialogInspPoint)

public:
	CDialogInspPoint(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDialogInspPoint();

// ��ȭ ���� �������Դϴ�.
#ifdef TABLET
	enum { IDD = IDD_DIALOG_INSP_POINT };
#else
#ifdef MODEL_BLOOM
	enum { IDD = IDD_DIALOG_INSP_POINT_TOUCH_BLOOM };
#else
	enum { IDD = IDD_DIALOG_INSP_POINT_TOUCH };
#endif
#endif
private:
	CBtnenhctrl m_InspPoint[MAX_INSP_POINT_CNT];
	CBtnenhctrl m_InspPoint2[MAX_INSP_POINT_CNT];

public:
	void SetInspRsltPointView(int nInspPoint, int nRslt = INIT_RSLT_BTN);
	void InitInspPoint();

	void SetEachRsltPoint(int nInspPoint, int nRslt = 3);

protected:
	void InspRsltPointView(int nInspPoint, int nRslt = INIT_RSLT_BTN);
	

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
