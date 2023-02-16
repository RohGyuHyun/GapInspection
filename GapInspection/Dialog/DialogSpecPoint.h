#pragma once


// CDialogSpec ��ȭ �����Դϴ�.

class CDialogSpecPoint : public CDialogEx
{
	DECLARE_DYNAMIC(CDialogSpecPoint)

public:
	CDialogSpecPoint(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDialogSpecPoint();

// ��ȭ ���� �������Դϴ�.
#ifdef MODE_BLOOM
	enum { IDD = IDD_DIALOG_SPEC_POINT_BLOOM };
#else
	enum { IDD = IDD_DIALOG_SPEC_POINT };
#endif

private:
	TypeSpecPara m_Spec[MAX_INSP_POINT_CNT];
	TypeSpecPara m_Spread[MAX_INSP_POINT_CNT];
	CBtnenhctrl m_ctrMinSpec[MAX_INSP_POINT_CNT];
	CBtnenhctrl m_ctrMaxSpec[MAX_INSP_POINT_CNT];
	int m_nSpecToSpreadIdx;
	BOOL m_isSave;
public:
	void GetSpec(TypeSpecPara *Spec);
	void SetSpec(TypeSpecPara *Spec);
	//20190424 ngh
	void GetSpread(TypeSpecPara *Spread);
	void SetSpread(TypeSpecPara *Spread);
	BOOL GetSave(){return m_isSave;};
protected:
	void UpdatePara(BOOL isUpdate, int nSpecToSpreadIdx);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	DECLARE_EVENTSINK_MAP()
	void SpecInput();
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	void ClickBtnenhctrlSpecApply();
	void ClickBtnenhctrlSpecCancel();
	void ClickBtnenhctrlSpecToSpread0();
	void ClickBtnenhctrlSpecToSpread1();
};
