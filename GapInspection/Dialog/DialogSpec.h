#pragma once

#include "DialogSpecPoint.h"
#include "DialogSpecPointOffset.h"
// CDialogSpec ��ȭ �����Դϴ�.

class CDialogSpec : public CDialogEx
{
	DECLARE_DYNAMIC(CDialogSpec)

public:
	CDialogSpec(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDialogSpec();

// ��ȭ ���� �������Դϴ�.
#ifdef MODEL_BLOOM
	enum { IDD = IDD_DIALOG_SPEC_BLOOM };
#else
	enum { IDD = IDD_DIALOG_SPEC };
#endif

private:
	TypeSpecPara m_Spec[MAX_RSLT_POINT_CNT];
	TypeSpecPara m_Spread[MAX_RSLT_POINT_CNT];
	CBtnenhctrl m_ctrMinSpec[MAX_RSLT_POINT_CNT];
	CBtnenhctrl m_ctrMaxSpec[MAX_RSLT_POINT_CNT];
	int m_nSpecToSpreadIdx;
	BOOL m_isSave;
	CDialogSpecPointOffset *m_pdlgPointOffset;
	TypeSpecPara m_PointOffset[MAX_INSP_POINT_CNT];

public:
	void GetSpec(TypeSpecPara *Spec);
	void SetSpec(TypeSpecPara *Spec);
	//20190424 ngh
	void GetSpread(TypeSpecPara *Spread);
	void SetSpread(TypeSpecPara *Spread);
	BOOL GetSave(){return m_isSave;};
	void GetPointOffset(TypeSpecPara *Spec);
	void SetPointOffset(TypeSpecPara *Spec);
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
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	void ClickBtnenhctrlSpecToSpread0();
	void ClickBtnenhctrlSpecToSpread1();
	void ClickBtnenhctrlSpecOffset();
};
